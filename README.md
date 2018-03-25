# Oreo device ID Changer

## A tool to change Device ID on Oreo

**Android 8 (Oreo) has a major behavior change on how device ID is manipulated, this tool helps you Query / Modify the ID of applications on Oreo**

## Usage

| Options | Arguments | Description |
| ------- | -------- | ----------- |
| -q, --query_current |  | Set this flag to query current ID (May be inherited from the previous system) |
| -r, --query_default |  | Set this flag to query the default ID (The new ID decided by Oreo) |
| -a, --assign_current | [ID] | To change current ID (Usually a 16-digits number) |
| -o, --assign_default | [ID] | To change the default ID |
| -p, --package | [PKG_NAME] | To specify application by package name, **THIS FLAG IS NECESSARY** |
| -i, --inplace |  | Mdify ID directly (Print to stdout if not set, **USE THIS OPTION CAREFULLY**) |
| -f, --file | [FILE_PATH] | To Specify which file to be processed, don't set to use default |
| -h, --help |  | Help information is currently NOT available |

## Notice
* Root privilege is needed.
* A **REBOOT** is required for changes to go into effect.
* Querying both current and default ID at the same time is NOT possible.
* To Assign the current and default ID at the same time, set the both flags.
* It is NOT possible to query and assign at the same time.
* Specify ONLY ONE package, the last one works if more than one is given.

## Examples
* To query the current ID of the package 'com.android.example'  
`dvcid -q -p com.android.example`
* To change the current ID of the package 'com.android.example' to '0000000000000000' directly  
`dvcid -i -a 0000000000000000 -p com.android.example`
* To query the current ID of the package 'com.android.example' from the file 'settings-ssaid.xml'  
`dvcid -q -p com.android.example -f settings-ssaid.xml`
* To set both ID of the package 'com.android.example' at the same time to 1234567890123456 and 0000000000000000  
`dvcid -a 1234567890123456 -o 0000000000000000 -p com.android.example`
