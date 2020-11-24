# Regular Expressions 101 (Desktop Edition)

![Regular Expressions 101](https://user-images.githubusercontent.com/55795671/100066099-86352280-2e2c-11eb-8ba1-858b3dad057d.png)

This project is a desktop version of the [regex101.com](https://regex101.com) site.  It embeds a copy of the site inside the application so that no internet connection is required to work on regular expressions.  The motivation behind this was that when developing regular expressions for use in products, I often find myself opening the website again and again, resulting in many tabs in various states swamped in the sea of web pages.

By embedding the website in an application, regex101 can be accessed immediately from your task bar, no more searching through tabs to find the right one!

In addition, the software redirects the regex101.com online database to a local SQLite database, so you can store your own personal library of regular expressions inside the application.

This is not an official regex101 product, but the author of the site gave his permission for me to release this application, so please support his hard work by making a donation if you find this application useful.

## Installation

Binary distributions can be found under the assets on the [github releases](https://github.com/fizzyade/regex101/releases) page.

- **Windows**.  The application is supplied as an installer executable, download and run the installer to install the application, this will create a shortcut which can be used to launch the software.

- **macOS**.  The application is supplied as a dmg disk image.  Download and open the disk image and drag the Regular Expressions 101 icon into the Applications folder, the application can then be launched by double clicking on the Regular Expressions 101 icon in Applications.

- **Linux**.  The application is supplied as an AppImage.  Download the application and then from the terminal run the command:

  `chmod +x <downloaded filename>`

## Requirements (Development)

- Qt 5 libraries (Requirement for development)
- Qt Creator (Optional development environment)

## Development

The application attempts to use as much of the original website files without modification of the copies that are stored in the binary.  The software does however make changes to the files as needed when it serves the files to the renderer, if you update the website files you will need to make sure that you have grabbed all the required files and will have to fix any changes that break compatability with this application.

### General Information

The following configurations are used for development.

- ***Linux*** - 32 & 64-bit using GNU g++
- ***Windows*** - 32 & 64-bit using MSVC 2017 Community Edition
- ***Windows*** - 32 & 64-bit using mingw-g++
- ***macOS*** - 64-bit using clang from xcode command line tools

### Deployment

The deploy.py python script is used to create runnable/installable binaries for the given operating system.

- ***Linux*** - Generates a stand-alone AppImage for easy use.
- ***Windows*** - Generates an installer for easy deployment.
- ***macOS*** - Generates a DMG file for easy deployment.

The script requires python 3.6 or later, curl and the [colorama](https://github.com/tartley/colorama) python module to enhance the console output, the colorama module can be installed using pip:

`pip3 install colorama`

The script provides the following parameters:

- `--qtdir="<path to qt>"` - the path to the version of qt that the application was built against
- `--curlbin="<path to curl binary>"` - the path to the curl binary, i.e `"/usr/bin/curl"`.
- `--arch="<x64|x86>"` - the architecture to deploy
- `--type=<release|debug>` - the build type to deploy
- `--cert="<cert>"` - the value of this is dependent on the platform, for Windows and macOS it is the name of the certificate to be used, for Linux it is the gpg2 key identifier.
- `--timeserver="<name>"` - ***Windows only***, the name of the time server to be used during signing
- `--appleid="<email>"` - ***macOS only***, the Apple ID of the developer account used to sign the binary
- `--password="<password>"` - ***macOS only***, the password of the Apple ID.  The password can be stored in the keychain and accessed using `"@keychain:<identifier>"` instead of passing the actual password to the script.

The script will use the curl binary to obtain any tools required for the deployment process.

The resulting asset will be placed in the deployment folder.

#### Windows

The installer and the application binaries are signed with my development certificate, the end user can validate the authenticity of the application with this certificate.

#### Linux

A hash is provided on the AppImage, the end user can use this to validate the authenticity of the application.

#### macOS

The application and the DMG are both signed with my Apple Developer certificate.

# Credits

The main credit goes to the author of the [regex101.com](https://regex101.com) website, it is an amazing tool that is the best I've used for working on regular expressions, please consider supporting the author by donating to him.

- [regex101.com](https://regex101.com) 

The following third party libraries/assets/tools/services have been used in the development of Regular Expressions 101 Desktop Edition.

- [Qt](https://www.qt.io/download) - cross platform framework, licensed under the [GPLv3](https://www.gnu.org/licenses/gpl-3.0.en.html).
- [macdeployqtfix](https://github.com/arl/macdeployqtfix) - finishes the job that macdeployqt starts, licensed under the [MIT License](https://github.com/arl/macdeployqtfix/blob/master/LICENSE).
- [linuxdeployqt](https://github.com/probonopd/linuxdeployqt) - creates deployable linux binaries, licensed under the [GPLv3 License](https://github.com/probonopd/linuxdeployqt/blob/master/LICENSE.GPLv3).
- [create-dmg](https://github.com/andreyvit/create-dmg) - automates the creation of macOS DMG files, licensed under the [MIT License](https://github.com/andreyvit/create-dmg/blob/master/LICENSE).
- [colorama](https://github.com/tartley/colorama) - python module for terminal colour, licensed under the [BSD License](https://github.com/tartley/colorama/blob/master/LICENSE.txt).

In addition, the following commercially licensed tools/services have also been used.

- [SmartCard Tools](https://www.mgtek.com/smartcard) - code signing tool, allows automation of signing using a smartcard token.
- [Certum Code Signing Certificate](https://en.sklep.certum.pl/data-safety/code-signing-certificates/open-source-code-signing-1022.html) - open source code signing certificate for signing Windows binaries.
- [AdvancedInstaller](https://www.advancedinstaller.com/) - Installer creator for windows, license kindly provided by them for free.
- [Affinity Designer](https://www.serif.com/designer) - Vector artwork design application.
- [Affinity Photo](https://www.serif.com/photo) - Bitmap artwork design application.

# License

This project is open source and is released under the GPLv3 License

Distributed as-is; no warranty is given.