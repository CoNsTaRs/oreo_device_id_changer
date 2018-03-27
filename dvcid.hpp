#ifndef DVCID_HPP_
#define DVCID_HPP_

#include <string>

struct ParsedLine
{
  ParsedLine () = default;

  ParsedLine
    (
      const std::string& id_A_,
      const std::string& name_A_,
      const std::string& value_A_,
      const std::string& package_A_,
      const std::string& default_value_A_,
      const std::string& default_sys_set_A_,
      const std::string& tag_A_
    )
    : id(id_A_), name(name_A_), value(value_A_), package(package_A_),
      default_value(default_value_A_), default_sys_set(default_sys_set_A_),
      tag(tag_A_)
  {}

  bool
  is_crrupted() const
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

  std::string id;
  std::string name;
  std::string value;
  std::string package;
  std::string default_value;
  std::string default_sys_set;
  std::string tag;
};

struct Options
{
  Options
    (
      const std::string& pkg_name = "",
      const std::string& dft_id = "",
      const std::string& crt_id = "",
      const std::string& xml_pth = "/data/system/users/0/settings_ssaid.xml",
      const std::string& bak_pth = "settings_ssaid.xml.bak",
      const std::string& rst_pth = "",
      bool to_help = false,
      bool to_as_dft = false,
      bool to_as_crt = false,
      bool to_qr_dft = false,
      bool to_qr_crt = false,
      bool to_backup = false,
      bool to_restore = false,
      bool do_inplace = false,
      bool do_forcibly = false
    )
    : package_name(pkg_name), default_id(dft_id), current_id(crt_id),
      xml_file(xml_pth), backup_file(bak_pth), restore_file(rst_pth),
      help(to_help), assign_default(to_as_dft), assign_current(to_as_crt),
      query_default(to_qr_dft), backup(to_backup), restore(to_restore),
      query_current(to_qr_crt), inplace(do_inplace), force(do_forcibly)
  {}

  std::string
    package_name,
    default_id,
    current_id,
    xml_file,
    backup_file,
    restore_file;

  bool
    help,
    assign_default,
    assign_current,
    query_default,
    query_current,
    backup,
    restore,
    inplace,
    force;
};

void parse_file(const std::string& xml_file);
void write_back(const std::string& final_copy, const std::string& xml_file);
void write_back(std::string&& final_copy, const std::string& xml_file);
void print_out(const std::string& final_copy);
void print_out(std::string&& final_copy);
std::string query(const std::string& package_name, const bool& which);
void assign_safe_guard(const std::string& package_name, const std::string& device_id);
void assign(const std::string& package_name, const std::string& device_id, const bool&& which);
void copy_file(const std::string& src_path, const std::string& dst_path);
void help_information(std::string&& exit_info = "", int&& error_code = 0);
Options get_options (int& argc, char** (&argv));
std::string get_final(bool&& committed = true);

#define __HELP_INFO__ "\
dvcid could help you query/modify Device IDs on Android 8 (Oreo)\n\
\n\
For further information about dvcid, see the page: https://github.com/CoNsTaRwU/oreo_device_id_changer/blob/master/README.md\n\
\n\
SYNOPSIS:\n\
    dvcid [FLAG] [ARGUMENT] [FLAG] [ARGUMENT] ...\n\
\n\
    Some Flags may not require an argument\n\
\n\
ARGUMENTS:\n\
    [ID]                                      Designated device ID to assign, usually a 16-digit number\n\
    [PACKAGE_NAME]                            Package name of the designated application\n\
    [FILE_PATH]                               Path to the file to be processed\n\
\n\
FLAGS:\n\
    -q, --query_current                       Set this flag to query the current ID (May be inherited from the previous Android version)\n\
    -Q, --query_default                       Set this flag to query the default ID (The new ID decided by Oreo)\n\
    -a, --assign_current    [ID]              To change the current ID\n\
    -A, --assign_default    [ID]              To change the default ID\n\
    -p, --package           [PACKAGE_NAME]    To specify application by package name, THIS FLAG IS NECESSARY\n\
    -b, --backup            [FILE_PATH]       To make a backup, output to the file specified\n\
    -r, --restore           [FILE_PATH]       To restore device ID from a backup\n\
    -i, --inplace                             To modify ID directly (Print to stdout if not set, USE THIS OPTION CAREFULLY)\n\
    -y, --force                               To skip security checks\n\
    -f, --file              [FILE_PATH]       To specify which file to be processed\n\
    -h, --help                                To show this information\n\
\n\
EXAMPLES:\n\
    To query the current ID of the package 'com.android.example':\n\
    dvcid -q -p com.android.example\n\
\n\
    To change the current ID of the package 'com.android.example' to '0000000000000000' directly:\n\
    dvcid -i -a 0000000000000000 -p com.android.example\n\
\n\
    To query the current ID of the package 'com.android.example' from the file 'device_id':\n\
    dvcid -q -p com.android.example -f device_id\n\
\n\
    To set both ID of the package 'com.android.example' at the same time to '1234567890123456' and '0000000000000000':\n\
    dvcid -a 1234567890123456 -A 0000000000000000 -p com.android.example\n\
\n\
    To save current settings to file 'device_id.bak':\n\
    dvcid -b ssaid_backup.xml\n\
\n\
    To restore IDs from a backup file 'device_id.bak'\n\
    dvcid -r device_id.bak\n\
\n\
NOTICE:\n\
    Root privilege is needed.\n\
    A REBOOT is required for changes to go into effect.\n\
    A valid (uncorrupted) setting is expected, otherwise the behavior will be undefined.\n\
    Always preview before assigning inplacely, or make backups.\n\
    Querying both current and default ID at the same time is NOT possible.\n\
    To Assign the current and default ID at the same time, set the both flags.\n\
    It is NOT possible to query and assign at the same time.\n\
    Specify ONLY ONE package, the last one works if more than one is given.\n\n\
";

static const option long_opts_SG_ [] =
{
  {"help",           no_argument,       nullptr, 'h'},
  {"assign_current", required_argument, nullptr, 'a'},
  {"assign_default", required_argument, nullptr, 'A'},
  {"query_current",  no_argument,       nullptr, 'q'},
  {"query_default",  no_argument,       nullptr, 'Q'},
  {"backup",         required_argument, nullptr, 'b'},
  {"restore",        required_argument, nullptr, 'r'},
  {"package",        required_argument, nullptr, 'p'},
  {"file",           required_argument, nullptr, 'f'},
  {"inplace",        no_argument,       nullptr, 'i'},
  {"force",          no_argument,       nullptr, 'y'},
  {nullptr, 0, nullptr, 0}
};

static const char opts_SG_ [] = "ha:o:qrb:r:p:f:iy";

#endif  // DVCID_HPP_
