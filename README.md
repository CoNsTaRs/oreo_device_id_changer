# Oreo device ID Changer

## A tool to change Device ID on Oreo

**Android 8 (Oreo) has a major behavior change on how device ID is manipulated, this tool helps you Query / Modify the ID of applications on Oreo**

## Usage

| Options | Arguments | Description |
| ------- | -------- | ----------- |
| -q, --query_current |  | To query current ID (May be inherited from the previous Android version) |
| -r, --query_default |  | To query the default ID (The new ID decided by Oreo) |
| -a, --assign_current | [ID] | To change current ID (Usually a 16-digits number) |
| -o, --assign_default | [ID] | To change the default ID |
| -b, --backup | [FILE_PATH]* | To make a backup, output to the file if specified, else print to stdout |
| -p, --package | [PKG_NAME] | To specify application by package name, **THIS FLAG IS NECESSARY** |
| -i, --inplace |  | Modify ID directly (Print to stdout if not set, **USE THIS OPTION CAREFULLY**) |
| -y, --force |  | To skip security checks |
| -f, --file | [FILE_PATH] | To Specify which file to be processed, don't set to use default |
| -h, --help |  | To show help information |

## Notice
* Root privilege is needed.
* A **REBOOT** is required for changes to go into effect.
* A valid (uncorrupted) setting is expected, otherwise the behavior will be undefined.
* Always preview before assigning inplacely, or make backups.
* Invalid option combinations will trigger help_information() and NO action will be performed.
* Specify ONLY ONE package, the last one works if more than one is given.

## Examples
* To query the current ID of the package 'com.android.example'  
`dvcid -q -p com.android.example`
* To change the current ID of the package 'com.android.example' to '0000000000000000' directly  
`dvcid -i -a 0000000000000000 -p com.android.example`
* To query the current ID of the package 'com.android.example' from the file 'settings-ssaid.xml'  
`dvcid -q -p com.android.example -f settings-ssaid.xml`
* To set both ID of the package 'com.android.example' at the same time to '1234567890123456' and '0000000000000000'  
`dvcid -a 1234567890123456 -o 0000000000000000 -p com.android.example`
* To save current settings to file 'ssaid_backup.xml'
`dvcid -b ssaid_backup.xml`
