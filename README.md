# Oreo device ID Changer

## A tool to change Device ID on Oreo

**Android 8 (Oreo) has a major behavior change on how device id is manipulated, this tool helps you Query / Modify the ID of applications on Oreo**

## Usage:

| Options | Arguments | Description |
| ------- | -------- | ----------- |
| -q, --query_current |  | Set this flag to query current ID (May be inherited from the previous system) |
| -r, --query_default |  | Set this flag to query the default ID (The new ID decided by Oreo) |
| -a, --assign_current | [ID] | To change current ID |
| -o, --assign_default | [ID] | To change the default ID |
| -p, --package | [PKG_NAME] | To specify application by package name, **THIS FLAG IS NECESSARY** |
| -i, --inplace |  | To modify ID directly (Print to stdout if not set, use this option carefully) |
| -f, --file | [FULL_PATH] | To Specify which file to be processed, don't set to use default |
| -h, --help |  | Help information is currently NOT available |

## Notice
1) Querying both current and default IDs at the same time is NOT possible.
2) It is NOT possible to query and assign at the same time.
3) To assign the current and default ID at the same time, set the both flags.
4) Specify ONLY ONE package, the last one works if more than one is given.
