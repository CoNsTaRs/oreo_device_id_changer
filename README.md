# Oreo device ID Changer

## A tool to change Device ID on Oreo

**Android 8 (Oreo) has a major behavior change on how device ID is manipulated, this tool helps you Query / Modify the ID of applications on Oreo**

# Flags changed, prior versions use different flags

## Usage

| Options | Arguments | Description |
| ------- | -------- | ----------- |
| -q, --query |  | To query current ID (May be inherited from the previous Android version) |
| -Q, --query_default |  | To query the default ID (The new ID decided by Oreo) |
| -a, --assign | [ID] | To change current ID (Usually a 16-digits number) |
| -A, --assign_default | [ID] | To change the default ID |
| -b, --backup | [FILE_PATH] | To make a backup, output to the file specified |
| -r, --restore | [FILE_PATH] | To restore from a backup file |
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
`dvcid --query --package com.android.example`
* To change the current ID of the package 'com.android.example' to '0000000000000000' directly  
`dvcid -i --assign 0000000000000000 --package com.android.example`
* To query the current ID of the package 'com.android.example' from the file 'settings-ssaid.xml'  
`dvcid --query --package com.android.example --file settings-ssaid.xml`
* To set both ID of the package 'com.android.example' at the same time to '1234567890123456' and '0000000000000000'  
`dvcid --assign 1234567890123456 -A 0000000000000000 --package com.android.example`
* To save current settings to file 'ssaid_backup.xml'  
`dvcid --backup ssaid_backup.xml`
