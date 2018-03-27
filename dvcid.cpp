#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <cstdlib>
#include <getopt.h>

#include "dvcid.hpp"

namespace {

  static std::deque<ParsedLine> parsed_XML_SG_;
  static std::deque<std::string> orign_XML_SG_;

}

int
main(int argc, char* argv[])
{
  Options opts = get_options(argc, argv);

  if (opts.help)
    { help_information("", 0); }

  /* At least one action needs to be taken */
  if (!(opts.query_default || opts.query_current || opts.assign_default ||
        opts.assign_current || opts.backup || opts.restore))
    { help_information("invalid option combination: nothing to do: ", 64); }

  /* Only one ID can be queried at a time */
  if (opts.query_default && opts.query_current)
    {
      help_information
        ("invalid option combination: querying more than one IDs: ", 64);
    }

  /* Backup, restore cannot be performed with query */
  if (
      (opts.backup || opts.restore) &&
      (opts.query_default || opts.query_current)
     )
    { help_information("invalid option combination: ", 64); }

  /* Cannot restore while assigning */
  if (opts.restore && (opts.assign_current || opts.assign_default))
    { help_information("invalid option combination: ", 64); }

  /* Cannot query and assign at one time */
  if (
      (opts.query_current || opts.query_default) &&
      (opts.assign_current || opts.assign_default)
     )
    { help_information("invalid option combination: ", 64); }

  /* Action combination is safe here, option combination still needs to check */

  /* Querying */
  if (opts.query_current || opts.query_default)
    {
      parse_file(opts.xml_file);

      if (opts.inplace)
        { help_information("invalid option combination: ", 64); }

      if (opts.package_name.length() == 0)
        { help_information("package name not spefified: ", 64); }

      std::string id = query(opts.package_name, opts.query_default);

      if (id.length() != 0)
        { std::cout << id << std::endl; }
    }

  /* Backing up */
  if (opts.backup)
    { copy_file(opts.xml_file, opts.backup_file); }

  /* Assigning */
  if (opts.assign_current || opts.assign_default)
    {
      parse_file(opts.xml_file);

      if (opts.package_name.length() == 0)
        { help_information("package name not spefified: ", 64); }

      if (opts.assign_default)
        {
          if (!opts.force)
            { assign_safe_guard(opts.package_name, opts.default_id); }
          assign(opts.package_name, opts.default_id, false);
        }
      if (opts.assign_current)
        {
          if (!opts.force)
            { assign_safe_guard(opts.package_name, opts.current_id); }
          assign(opts.package_name, opts.current_id, true);
        }
    }

  /* Writing back */

  if (opts.assign_current || opts.assign_default)
    {
      if (opts.inplace)
        { write_back(get_final(), opts.xml_file); }
      else
        { print_out(get_final()); }
    }

  if (opts.restore)
    { copy_file(opts.restore_file, opts.xml_file); }

  return 0;
}

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
      ::parsed_XML_SG_.push_back(::parse_line(line));
      ::orign_XML_SG_.push_back(line);
    }

  input_stream.close();
}

std::string
get_final(bool&& committed)
{
  std::string final_copy;
  for (std::size_t i = 0; i < ::orign_XML_SG_.size(); ++i)
    {
      final_copy.append
        (
          (::parsed_XML_SG_[i].is_crrupted() || !committed) ?
            ::orign_XML_SG_[i] : ::rewrite_line(::parsed_XML_SG_[i])
        )
      .append("\n");
    }
  return final_copy;
}

void
write_back(const std::string& final_copy, const std::string& xml_file)
{
  std::ofstream output_stream(xml_file);

  if (output_stream.fail())
    {
      std::cerr << "can't create (user) output file: " << xml_file << std::endl;
      exit(73);
    }

  output_stream << final_copy;
  output_stream.close();
}

void
write_back(std::string&& final_copy, const std::string& xml_file)
{
  std::ofstream output_stream(xml_file);

  if (output_stream.fail())
    {
      std::cerr << "can't create (user) output file: " << xml_file << std::endl;
      exit(73);
    }

  output_stream << final_copy;
  output_stream.close();
}

void
print_out(const std::string& final_copy)
{
  std::cout << final_copy;
}

void
print_out(std::string&& final_copy)
{
  std::cout << final_copy;
}

std::string
query(const std::string& package_name, const bool& which)
{
  for (auto const& parsed_line : ::parsed_XML_SG_)
    {
      if (parsed_line.package == package_name)
        { return (which ? parsed_line.default_value : parsed_line.value); }
    }

  return std::string();
}

void
assign_safe_guard(const std::string& package_name, const std::string& device_id)
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
    const bool&& which
  )
{
  for (auto& parsed_line : ::parsed_XML_SG_)
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
copy_file(const std::string& src_path, const std::string& dst_path)
{
  std::ifstream inpt_stream(src_path, std::ios::binary);
  std::ofstream oupt_stream(dst_path, std::ios::binary);

  if (inpt_stream.fail() || oupt_stream.fail())
    {
      if (inpt_stream.fail())
        { std::cerr << "cannot open input: " << src_path << std::endl; }
      if (oupt_stream.fail())
        { std::cerr << "cannot open output: " << dst_path << std::endl; }
      exit(73);
    }

  oupt_stream << inpt_stream.rdbuf();

  inpt_stream.close();
  oupt_stream.close();
}

void
help_information(std::string&& exit_info, int&& error_code)
{
  if (exit_info.length() > 0)
    { std::cout << exit_info << error_code << std::endl << std::endl; }
  std::cout << __HELP_INFO__;
  exit(error_code);
}

Options
get_options (int& argc, char** (&argv))
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
      case 'A':
        options.assign_current = true;
        options.current_id = optarg;
        break;
      case 'q':
        options.query_default = true;
        break;
      case 'Q':
        options.query_current = true;
        break;
      case 'b':
        options.backup = true;
        options.backup_file = optarg;
        break;
      case 'r':
        options.restore = true;
        options.restore_file = optarg;
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
