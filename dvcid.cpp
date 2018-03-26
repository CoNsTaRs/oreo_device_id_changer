#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <cstdlib>
#include <getopt.h>

#include "dvcid.hpp"

static std::deque<ParsedLine> parsed_XML_SG_;
static std::deque<std::string> orign_XML_SG_;

namespace {

  std::string
  parse_key(std::size_t p, const std::string& line)
  {
    std::string parsed_key;
    bool found_key = false;

    for (p = p - 1; p != 0; --p)
      {
        if (line[p] != ' ')
          {
            found_key = true;
            parsed_key.insert(parsed_key.begin(), line[p]);
          }
        else if (found_key)
          { break; }
      }

    return parsed_key;
  }

  std::string
  parse_value (std::size_t p, const std::string& line)
  {
    std::string parsed_value;
    bool found_value = false;

    for (p = p + 2; p < line.length(); ++p)
      {
        if (line[p] != '"')
          {
            found_value = true;
            parsed_value.push_back(line[p]);
          }
        else if (found_value)
          { break; }
      }

    return parsed_value;
  }

  ParsedLine
  parse_line(const std::string& line)
  {
    std::map<std::string, std::string> records;

    for (std::size_t i = 0; i < line.length(); ++i)
      {
        if (line[i] == '=')
          {
            std::string key = ::parse_key(i, line);
            std::string value = ::parse_value(i, line);
            records[key] = value;
          }
      }

    return ParsedLine
      (
        records["id"], records["name"], records["value"], records["package"],
        records["defaultValue"], records["defaultSysSet"], records["tag"]
      );
  }

  std::string
  rewrite_line(const ParsedLine& parsed_line)
  {
    return std::string()
      .append("  <setting ")
      .append("id=\"")              .append(parsed_line.id)
      .append("\" name=\"")         .append(parsed_line.name)
      .append("\" value=\"")        .append(parsed_line.value)
      .append("\" package=\"")      .append(parsed_line.package)
      .append("\" defaultValue=\"") .append(parsed_line.default_value)
      .append("\" defaultSysSet=\"").append(parsed_line.default_sys_set)
      .append("\" tag=\"")          .append(parsed_line.tag)
      .append("\" />");
  }

  std::string
  get_final(bool committed = true)
  {
    std::string final_copy;
    for (std::size_t i = 0; i < orign_XML_SG_.size(); ++i)
      {
        final_copy.append
          (
            (parsed_XML_SG_[i].is_crrupted() || !committed) ?
              orign_XML_SG_[i] : ::rewrite_line(parsed_XML_SG_[i])
          )
        .append("\n");
      }
    return final_copy;
  }

  bool
  is_valid_device_id(const std::string& str)
  {
    if (str.length() != 16)
      { return false; }
    for (const auto& c : str)
      {
        if (static_cast<int>(c) < 48 || static_cast<int>(c) > 57)
          { return false; }
      }
    return true;
  }

  bool
  is_valid_userkey(const std::string& str)
  {
    if (str.length() != 64)
      { return false; }
    for (auto it = str.cbegin(); it != str.cend(); ++it)
      {
        if (
              (static_cast<int>(*it) > 57 && static_cast<int>(*it) < 65) ||
              (static_cast<int>(*it) < 48 || static_cast<int>(*it) > 90)
           )
          { return false; }
      }
    return true;
  }
}

void
parse_file(const std::string& xml_file)
{
  std::ifstream input_stream(xml_file);

  if (input_stream.fail())
    {
      std::cerr << "cannot open input: " << xml_file << std::endl;
      exit(66);
    }

  for (std::string line; std::getline(input_stream, line); )
    {
      parsed_XML_SG_.push_back(::parse_line(line));
      orign_XML_SG_.push_back(line);
    }
}

void
write_back(std::string final_copy, const std::string& xml_file)
{
  std::ofstream output_stream(xml_file);

  if (output_stream.fail())
    {
      std::cerr << "can't create (user) output file: " << xml_file << std::endl;
      exit(73);
    }
  output_stream << final_copy;
}

void
print_out(std::string final_copy)
{
  std::cout << final_copy;
}

std::string
query(const std::string& package_name, const bool which)
{
  for (auto const& parsed_line : parsed_XML_SG_)
    {
      if (parsed_line.package == package_name)
        { return (which ? parsed_line.default_value : parsed_line.value); }
    }

  return std::string();
}

void
assign_safe_guard
  (
    const std::string& package_name,
    const std::string& device_id
  )
{
  if (package_name == "android")
    {
      if (!::is_valid_userkey(device_id))
        {
          std::cerr
            << "the designated ID is NOT a valid userkey: "
            << package_name
            << std::endl
            << "use --force to perform anyway"
            << std::endl;
        }
      std::cerr
        << "changing ID of this package will cause system wide ID changes: "
        << package_name
        << std::endl
        << "use --force to perform anyway"
        << std::endl;
      exit(1);
    }
  else if (!::is_valid_device_id(device_id))
    {
      std::cerr
        << "the designated ID is NOT a 16-digit number: "
        << device_id
        << std::endl
        << "use --force to perform anyway"
        << std::endl;
      exit(1);
    }
}

void
assign
  (
    const std::string& package_name,
    const std::string& device_id,
    const bool which
  )
{
  

  for (auto& parsed_line : parsed_XML_SG_)
    {
      if (parsed_line.package == package_name)
        {
          which ?
            parsed_line.default_value = device_id : 
            parsed_line.value = device_id;
        }
    }
}

void
help_information(std::string exit_info = "", int error_code = 0)
{
  if (exit_info.length() > 0)
    { std::cout << exit_info << error_code << std::endl << std::endl; }
  std::cout << __HELP_INFO__;
  exit(error_code);
}

Options
get_options (int& argc, char* argv[])
{
  int c;
  Options options;

  while ((c = getopt_long(argc, argv, opts_SG_, long_opts_SG_, nullptr)) != -1)
    {
      switch (c)
      {
      case 'a':
        options.assign_default = true;
        options.default_id = optarg;
        break;
      case 'o':
        options.assign_current = true;
        options.current_id = optarg;
        break;
      case 'q':
        options.query_default = true;
        break;
      case 'r':
        options.query_current = true;
        break;
      case 'b':
        options.backup = true;
        options.backup_file = optarg;
        break;
      case 'p':
        options.package_name = optarg;
        break;
      case 'f':
        options.xml_file = optarg;
        break;
      case 'i':
        options.inplace = true;
        break;
      case 'y':
        options.force = true;
        break;
      case 'h':
        options.help = true;
        break;
      default:
        help_information("", 63);
      }
    }
  return options;
}

int
main(int argc, char* argv[])
{
  Options opts = get_options(argc, argv);

  if (opts.help)
    { help_information("", 0); }

  /* At least one action needs to be taken */
  if (!(opts.query_default || opts.query_current || opts.assign_default ||
        opts.assign_current ||opts.backup))
    { help_information("invalid option combination: nothing to do: ", 64); }

  /* Only one ID can be queried at a time */
  if (opts.query_default && opts.query_current)
    {
      help_information
        ("invalid option combination: querying more than one IDs: ", 64);
    }

  /* Either backup or query can be taken */
  if (opts.backup && (opts.query_default || opts.query_current))
    { help_information("invalid option combination: ", 64); }

  /* Action combination is safe here, option combination still needs to check */

  parse_file(opts.xml_file);

  /* Querying */
  if (opts.query_current || opts.query_default)
    {
      if (opts.inplace)
        { help_information("invalid option combination: ", 64); }

      if (opts.package_name.length() == 0)
        { help_information("package name not spefified: ", 64); }

      std::string id = query(opts.package_name, opts.query_default);

      if (id.length() != 0)
        { std::cout << id << std::endl; }
    }

  /* Backing up */
  std::string orign_xml_backup;
  if (opts.backup)
    { orign_xml_backup = ::get_final(false); }

  /* Assigning */
  if (opts.assign_current || opts.assign_default)
    {
      if (opts.package_name.length() == 0)
        { help_information("package name not spefified: ", 64); }

      if (opts.assign_default)
        {
          if (!opts.force)
            { assign_safe_guard(opts.package_name, opts.default_id); }
          assign(opts.package_name, opts.default_id, true);
        }
      if (opts.assign_current)
        {
          if (!opts.force)
            { assign_safe_guard(opts.package_name, opts.current_id); }
          assign(opts.package_name, opts.current_id, false);
        }
    }

  /* Writing back */

  if (opts.backup)
    {
      if (opts.backup_file.length() == 0)
        { print_out(orign_xml_backup); }
      else
        { write_back(orign_xml_backup, opts.backup_file); }
    }

  if (opts.inplace)
    { write_back(::get_final(), opts.xml_file); }
  else
    { print_out(::get_final()); }

  return 0;
}
