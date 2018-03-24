#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <map>
#include <cstdlib>
#include <getopt.h>

struct ParsedLine
{
  std::string id;
  std::string name;
  std::string value;
  std::string package;
  std::string default_value;
  std::string default_sys_set;
  std::string tag;

  bool
  crrupt() const
  {
    return (
      id.empty() &&
      name.empty() &&
      value.empty() &&
      package.empty() &&
      default_value.empty() &&
      default_sys_set.empty() &&
      tag.empty());
  }
};

struct Options
{
  Options (const std::string& pkg_name = ""
         , const std::string& dft_id = ""
         , const std::string& crt_id = ""
         , const std::string& xml_pth= "/data/system/users/0/settings_ssaid.xml"
         , bool to_help = false
         , bool to_as_dft = false
         , bool to_as_crt = false
         , bool to_qr_dft = false
         , bool to_qr_crt = false
         , bool do_inplace = false)
    : package_name(pkg_name), default_id(dft_id), current_id(crt_id)
    , xml_file(xml_pth), help(to_help), assign_default(to_as_dft)
    , assign_current(to_as_crt), query_default(to_qr_dft)
    , query_current(to_qr_crt), inplace(do_inplace)
  {}

  std::string
    package_name,
    default_id,
    current_id,
    xml_file;

  bool
    help,
    assign_default,
    assign_current,
    query_default,
    query_current,
    inplace;
};

static std::deque<ParsedLine> parsed_XML;
static std::deque<std::string> orign_XML;

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

  ParsedLine parsed_line =
    {
      .id              = records["id"],
      .name            = records["name"],
      .value           = records["value"],
      .package         = records["package"],
      .default_value   = records["defaultValue"],
      .default_sys_set = records["defaultSysSet"],
      .tag             = records["tag"]
    };

  return parsed_line;
}

std::string
rewrite_line(const ParsedLine& parsed_line)
{
  return std::string("  <setting ")
    .append("id=\"")              .append(parsed_line.id)
    .append("\" name=\"")         .append(parsed_line.name)
    .append("\" value=\"")        .append(parsed_line.value)
    .append("\" package=\"")      .append(parsed_line.package)
    .append("\" defaultValue=\"") .append(parsed_line.default_value)
    .append("\" defaultSysSet=\"").append(parsed_line.default_sys_set)
    .append("\" tag=\"")          .append(parsed_line.tag)
    .append("\" />\n");
}

void
parse_file(const std::string& xml_file)
{
  std::ifstream input_stream(xml_file);
  ParsedLine parsed_line;

  for (std::string line; std::getline(input_stream, line); )
    {
      parsed_line = parse_line(line);
      parsed_XML.push_back(parsed_line);
      orign_XML.push_back(line);
    }
}

void
write_back(const std::string& xml_file)
{
  std::ofstream output_stream(xml_file);

  for (std::size_t i = 0; i < orign_XML.size(); ++i)
    {
      output_stream <<
        (parsed_XML[i].crrupt() ? orign_XML[i] : rewrite_line(parsed_XML[i]));
    }
}

std::string
query(const std::string& package_name, const bool which)
{
  for (auto const& parsed_line : parsed_XML)
    {
      if (parsed_line.package == package_name)
        { return (which ? parsed_line.default_value : parsed_line.value); }
    }

  return std::string();
}

void
assign(const std::string& package_name
     , const std::string& device_id
     , const bool which)
{
  for (auto& parsed_line : parsed_XML)
    {
      if (parsed_line.package == package_name)
        {
          if (which)
            { parsed_line.default_value = device_id; }
          else
            { parsed_line.value = device_id; }
        }
    }
}

void
help_information()
{
  std::cout << "help information" << std::endl;
}

Options
get_options (int argc, char** argv)
{
  static const option long_options [] =
    {
      {"help",           no_argument,       nullptr, 'h'},
      {"assign_default", required_argument, nullptr, 'a'},
      {"assign_current",     required_argument, nullptr, 'o'},
      {"query_default",  no_argument,       nullptr, 'q'},
      {"query_current",      no_argument,       nullptr, 'r'},
      {"package",        required_argument, nullptr, 'p'},
      {"file",           required_argument, nullptr, 'f'},
      {"inplace",        no_argument,       nullptr, 'i'},
      {nullptr, 0, nullptr, 0}
    };

  int c;
  Options options;

  while ((c = getopt_long(argc, argv, "ha:o:qrp:f:i", long_options, nullptr)) != -1)
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
      case 'p':
        options.package_name = optarg;
        break;
      case 'f':
        options.xml_file = optarg;
        break;
      case 'i':
        options.inplace = true;
        break;
      case 'h':
        options.help = true;
        break;
      default:
        std::cerr << "invalid option: " << static_cast<int>(c) << std::endl;
        exit(128);
      }
    }
  return options;
}

int
main(int argc, char* argv[])
{
  Options opt = get_options(argc, argv);

  parse_file(opt.xml_file);

  if (opt.help ||
      ((opt.assign_default || opt.assign_current) &&
       (opt.query_default || opt.query_current)))
    {
      help_information();
    }
  else if (opt.assign_default || opt.assign_current)
    {
      if (opt.assign_default)
        { assign (opt.package_name, opt.default_id, true); }
      if (opt.assign_current)
        { assign (opt.package_name, opt.current_id, false); }
      if (opt.inplace)
        { write_back(opt.xml_file); }
      else
        {
          for (std::size_t i = 0; i < orign_XML.size(); ++i)
            {
              if (parsed_XML[i].crrupt())
                { std::cout << orign_XML[i] << std::endl; }
              else
                { std::cout << rewrite_line(parsed_XML[i]) << std::endl; }
            }
        }
    }
  else if (opt.query_default || opt.query_current)
    {
      if (opt.query_default == opt.query_current)
        {
          help_information();
        }
      else
        {
          std::string id;
          if (opt.query_default)
            { id = query(opt.package_name, true); }
          else
            { id = query(opt.package_name, false); }
          if (id.length() != 0)
            { std::cout << id << std::endl; }
        }
    }
  else
    {
      help_information();
    }

  return 0;
}

