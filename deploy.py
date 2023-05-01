#!/usr/bin/env python3
#
# Copyright (C) 2019 Adrian Carpenter
# 
# Multiplatform Qt Deployment Tool
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import platform
import os
import fnmatch
import shutil
import glob
import tempfile
import argparse
import re
import time
import subprocess
import sys
import logging
import datetime
import enum

if tuple(int(i) for i in platform.python_version_tuple())<(3,6,0):
    print('requires python >= 3.6')
    quit(1)

if platform.system()=="Darwin":
    import dmgbuild

deploymentProject = "Regular Expressions 101"

try:
    from colorama import Fore, Back, Style, init

    init(autoreset=True)

except ModuleNotFoundError:
    class Style:
        BRIGHT = ''
        RESET = ''

    class Fore:
        GREEN = ''
        RED = ''
        CYAN = ''
        RESET = ''

def timeDelta(seconds):
    seconds = abs(int(seconds))
    
    days, seconds = divmod(seconds, 86400)
    hours, seconds = divmod(seconds, 3600)
    minutes, seconds = divmod(seconds, 60)

    if days > 0:
        return(f'{days}d{hours}h{minutes}m{seconds}s')
    elif hours > 0:
        return(f'{hours}h{minutes}m{seconds}s')
    elif minutes > 0:
        return(f'{minutes}m{seconds}s')
    else:
        return(f'{seconds}s')

def startMessage(message):
    sys.stdout.write(Style.BRIGHT+f'> {message} ')

def endMessage(state, message=None):
    if state:
        if platform.system()=="Windows":
            sys.stdout.write(Style.BRIGHT+'['+Fore.GREEN+'Y'+Fore.RESET+']\r\n')
        else:
            sys.stdout.write(Style.BRIGHT+'['+Fore.GREEN+'✓'+Fore.RESET+']\r\n')
    else:
        if platform.system()=="Windows":
            sys.stdout.write(Style.BRIGHT+'['+Fore.RED+'N'+Fore.RESET+']\r\n')
        else:
            sys.stdout.write(Style.BRIGHT+'['+Fore.RED+'✘'+Fore.RESET+']\r\n')


    if not state and message:
        print('\r\n'+Fore.RED+'ERROR: '+Fore.RESET+message)

def parent(path):
    return(os.path.normpath(os.path.join(path, os.pardir)))

def execute(command):
    output = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return(output.returncode, output.stdout.decode('utf-8'))

def which(appname):
    if platform.system()=="Windows" :
        command = 'where'
    else:
        command = 'which'

    status, output = execute(f'{command} {appname}')

    if status and output:
        return(output.split()[0])

def run(command) :
    stream = os.popen(command)

    return stream.read()

def macSignBinary(file, options, cert):
    extraOptions = ""

    if options:
        for option in options:
            extraOptions += option+" "

    return(execute(f'codesign --verify --deep --timestamp -o runtime --force {extraOptions} --sign "{cert}" "{file}"'))

def winSignBinary(signtool, file, cert, timeserver):
    return(execute(f'{signtool} sign /n "{cert}" /t {timeserver} /fd sha256 /v "{file}"'))

def notarizeFile(file, username, password):
    uuidPattern = re.compile(r'RequestUUID\s=\s(?P<requestUUID>[a-f|0-9]{8}-[a-f|0-9]{4}-[a-f|0-9]{4}-[a-f|0-9]{4}-[a-f|0-9]{12})\n')
    statusPattern = re.compile(r'\s*Status:\s(?P<status>\w+)\n')

    filename = os.path.basename(file)

    uploadId = str(int(time.time()))

    resultCode,result = execute(f'xcrun altool --notarize-app --primary-bundle-id "com.fizzyade.{uploadId}" -u {username} --password {password} --file "{file}"')

    if resultCode:
        return(False)

    match = uuidPattern.search(result)

    if not match:
        return(False)

    requestId = match.groupdict()["requestUUID"]

    if not requestId:
        return(False)

    while True:
        resultCode,result = execute(f'xcrun altool --notarization-info {requestId} -u {username} --password {password}')

        if not resultCode:
            match = statusPattern.search(result)

            if match:
                status = match.groupdict()["status"]

                if status=="in progress":
                    time.sleep(10)
                    continue

                if status=="success":
                    break

                if status=="invalid":
                    break

    return(status)

def deployWindows(args):

    buildArch = args.arch
    buildType = args.type.capitalize()

    isRDP = os.environ['SESSIONNAME'].startswith('RDP-')

    # check curl is available

    startMessage('Checking for curl...')

    if args.curlbin:
        if os.path.isfile(args.curlbin):
            curl = args.curlbin
        else:
            curl = None
    else:
        curl = which('curl.exe')

    if not curl:
        endMessage(False, 'curl could not be found. (see --curlbin).')
        exit(1)

    endMessage(True)

    # check for qt installation

    startMessage('Checking qtdir...')

    if args.qtdir:
        if os.path.isfile(f'{args.qtdir}\\bin\\windeployqt.exe'):
            windeployqt = f'{args.qtdir}\\bin\\windeployqt.exe'
        else:
            windeployqt = None
    else:
        windeployqt = which('windeployqt')

    if not windeployqt:
        endMessage(False, 'qt could not be found. (see --qtdir).')
        exit(1)

    endMessage(True)

    # create tools folder if it doesn't exist

    startMessage('Setting up tools directory...')

    if not os.path.exists(f'tools'):
        os.makedirs(f'tools')

    endMessage(True)

    tempdir = os.path.normpath(tempfile.mkdtemp())

    signtool = args.signtool

    if args.cert:
        if not os.path.exists(signtool):
            startMessage('Downloading SmartCardTools...')

            resultCode, resultOutput = execute(f'cd \"{tempdir}\" && \"{curl}\" -LJO https://www.mgtek.com/files/smartcardtools.zip')

            if resultCode:
                endMessage(False, f'unable to download SmartCardTools.\r\n\r\n{resultOutput}\r\n')
                exit(1)

            resultCode, resultOutput = execute(f'cd \"{tempdir}\" && \"{curl}\" -LJO ftp://ftp.info-zip.org/pub/infozip/win32/unz600xn.exe & unz600xn -jo unzip.exe')

            if resultCode:
                endMessage(False, f'unable to download info-zip tools.\r\n\r\n{resultOutput}\r\n')
                exit(1)

            resultCode, resultOutput = execute(f'\"{tempdir}\\unzip\" \"{tempdir}\\smartcardtools.zip\" -d tools\\smartcardtools')

            if resultCode:
                endMessage(False, f'unable to unzip SmartCardTools.\r\n\r\n{resultOutput}\r\n')
                exit(1)

            signtool = 'tools\\smartcardtools\\x64\\ScSignTool.exe'

            endMessage(True)

    # remove previous deployment files and copy current binaries

    startMessage('Setting up deployment directory...')

    deployDir = f'bin\\{buildArch}\\Deploy'
    binaryDir = f'bin\\{buildArch}\\{buildType}'

    extensions=['.exe', '.dll']

    if os.path.exists(deployDir):
        shutil.rmtree(deployDir)

    signList = []

    os.makedirs(deployDir)

    for file in glob.glob(f'{binaryDir}\\**\\*', recursive=True):
        basename, extension = os.path.splitext(file)

        if os.path.isdir(file):
            os.makedirs(file.replace(binaryDir, deployDir, 1))

        if extension in extensions:
            destFile = file.replace(binaryDir, deployDir, 1)
            shutil.copy2(file, destFile)

            signList.append(destFile)

    files = []

    for extension in extensions:
        files += glob.glob(f'{deployDir}\\*{extension}')

    if not files:
        endMessage(False, 'no files could be found to deploy.')
        exit(1)

    endMessage(True)

    # sign the application binaries

    if args.cert:
        startMessage('Signing binaries...')

        if isRDP:
            endMessage(False, f'Unable to sign during an RDP session.')
            exit(1)

        for file in signList:
            resultCode, resultOutput = winSignBinary(signtool, file, args.cert, args.timeserver)

            if resultCode:
                endMessage(False, f'there was a problem signing a file ({file}).\r\n\r\n{resultOutput}\r\n')
                exit(1)

        endMessage(True)
    
    filesString = ''

    for file in files:
        filesString += f'{file} '

    filesString = filesString.strip()

    # run windeplotqt

    startMessage('Running windeployqt...')

    resultCode, resultOutput = execute(f'{windeployqt} --dir "{deployDir}" "{filesString}"')

    if resultCode:
        endMessage(False, f'there was a problem running windeployqt.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)    

    # run advanced installer

    startMessage('Creating installer...')

    resultCode, resultOutput = execute(f'AdvancedInstaller.com /build "installer\\{deploymentProject}.aip"')

    if resultCode:
        endMessage(False, f'there was a problem creating the installer.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)  

    # sign the installer file

    if args.cert:
        startMessage('Signing installer...')

        if isRDP:
            endMessage(False, f'Unable to sign during an RDP session.')
            exit(1)

        resultCode, resultOutput = winSignBinary(signtool, f'deployment\\{deploymentProject}.exe', args.cert, args.timeserver)

        if resultCode:
            endMessage(False, f'there was a problem signing the installer.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)

    endTime = time.time()

    # done!

    print(f'\r\n'+Style.BRIGHT+Fore.CYAN+f'Finished! Installer at "deployment\\{deploymentProject}.exe" is '+Fore.GREEN+'ready'+Fore.CYAN+' for distribution.')

def deployLinux(args):

    buildArch = args.arch
    buildType = args.type.capitalize()

    # check curl is available

    startMessage('Checking for curl...')

    if args.curlbin:
        if os.path.isfile(args.curlbin):
            curl = args.curlbin
        else:
            curl = None
    else:
        curl = which('curl')

    if not curl:
        endMessage(False, 'curl could not be found. (see --curlbin).')
        exit(1)

    endMessage(True)

    # check for qt installation

    startMessage('Checking qtdir...')

    if args.qtdir:
        if os.path.isfile(args.qtdir+'/bin/qmake'):
            qtdir = args.qtdir
        else:
            qtdir = None
    else:
        qmake = which('qmake')

        if qmake:
            qtdir = parent(os.path.normpath(os.path.dirname(qmake)))
        else:
            qtdir = None

    if not qtdir:
        endMessage(False, 'qt directory could not be found. (see --qtdir).')
        exit(1)
    else:
        if not os.path.isdir(qtdir):
            endMessage(False, 'qt directory could not be found. (see --qtdir).')
            exit(1)

    endMessage(True)

    # create tools folder if it doesn't exist

    startMessage('Setting up tools directory...')

    if not os.path.exists(f'tools'):
        os.makedirs(f'tools')

    endMessage(True)

    # download linuxdeployqt

    linuxdeployqt = args.linuxdeployqt

    if not os.path.isfile(linuxdeployqt) :
        startMessage('Downloading linuxdeployqt...')

        if not os.path.exists('tools/linuxdeployqt'):
            os.mkdir('tools/linuxdeployqt')

        resultCode, resultOutput = execute(f'cd tools/linuxdeployqt; curl -LJO https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-{hostArch}.AppImage')

        if resultCode:
            endMessage(False, f'unable to download linuxdeployqt.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        resultCode, resultOutput = execute(f'chmod +x tools/linuxdeployqt/linuxdeployqt-6-{hostArch}.AppImage')

        if resultCode:
            endMessage(False, f'unable to set permissions on linuxdeployqt.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        linuxdeployqt = f'tools/linuxdeployqt/linuxdeployqt-6-{hostArch}.AppImage'

        endMessage(True)

    appimagetool = args.appimagetool

    if not os.path.isfile(appimagetool):
        startMessage('Downloading appimagetool...')

        if not os.path.exists('tools/appimagetool'):
            os.mkdir('tools/appimagetool')

        resultCode, resultOutput = execute(f'cd tools/appimagetool; curl -LJO https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-{hostArch}.AppImage')

        if resultCode:
            endMessage(False, f'unable to download appimagetool.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        resultCode, resultOutput = execute(f'chmod +x tools/appimagetool/appimagetool-{hostArch}.AppImage')

        if resultCode:
            endMessage(False, f'unable to set permissions on appimagetool.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        appimagetool = f'tools/appimagetool/appimagetool-{hostArch}.AppImage'

        endMessage(True)

    # remove previous deployment files and copy current binaries

    startMessage('Setting up deployment directory...')

    if os.path.exists(f'bin/{buildArch}/Deploy/'):
        shutil.rmtree(f'bin/{buildArch}/Deploy/')

    if os.path.exists(f'deployment'):
        shutil.rmtree(f'deployment')

    os.makedirs(f'deployment')

    os.makedirs(f'bin/{buildArch}/Deploy/usr/bin')
    os.makedirs(f'bin/{buildArch}/Deploy/usr/lib')
    os.makedirs(f'bin/{buildArch}/Deploy/usr/share')
    os.makedirs(f'bin/{buildArch}/Deploy/usr/share/applications')

    shutil.copy2(f'bin/{buildArch}/{buildType}/{deploymentProject}', f'bin/{buildArch}/Deploy/usr/bin')
    shutil.copy2(f'installer/{deploymentProject}.png', f'bin/{buildArch}/Deploy/regex101.png')
    shutil.copy2(f'installer/{deploymentProject}.desktop', f'bin/{buildArch}/Deploy/{deploymentProject}.desktop')
    shutil.copy2(f'installer/AppRun', f'bin/{buildArch}/Deploy/')

    for file in glob.glob(f'bin/{buildArch}/{buildType}/*.so') :
        shutil.copy2(file, f'bin/{buildArch}/Deploy/usr/lib')

    endMessage(True)

    # create the app dir

    startMessage('Running linuxdeployqt...')

    resultCode, resultOutput = execute(f'{linuxdeployqt} "bin/{buildArch}/Deploy/usr/bin/{deploymentProject}" -qmake="{qtdir}/bin/qmake" -exclude-libs="libqsqlodbc,libqsqlpsql" -unsupported-allow-new-glibc')

    if resultCode:
        endMessage(False, f'there was a problem running linuxdeployqt.\r\n\r\n{resultCode}\r\n')
        exit(1)

    endMessage(True)

    # create the AppImage

    signParameters = ''

    if args.cert:
        signParameters = f'-s --sign-key={args.cert} '

    startMessage('Creating AppImage...')

    resultCode, resultOutput = execute(f'ARCH={buildArch} {appimagetool} -g {signParameters} bin/{buildArch}/Deploy "deployment/{deploymentProject}-{buildArch}.AppImage"')

    if resultCode:
        endMessage(False, f'there was a problem creating the AppImage.\r\n\r\n{resultCode}\r\n')
        exit(1)   

    endMessage(True)

    # done!

    print(f'\r\n'+Style.BRIGHT+Fore.CYAN+f'Finished! AppImage at "deployment/{deploymentProject}-{buildArch}.AppImage" is '+Fore.GREEN+'ready'+Fore.CYAN+' for distribution.')

def deployMacOS(args):

    buildArch = args.arch
    buildType = args.type.capitalize()

    # check for qt installation

    startMessage('Checking qtdir...')

    if args.qtdir:
        if os.path.isfile(args.qtdir+'/bin/qmake'):
            qtdir = args.qtdir
        else:
            qtdir = None
    else:
        qmake = which('qmake')

        if qmake:
            qtdir = parent(os.path.normpath(os.path.dirname(qmake)))
        else:
            qtdir = None

    if not qtdir:
        endMessage(False, 'qt directory could not be found. (see --qtdir).')
        exit(1)
    else:
        if not os.path.isdir(qtdir):
            endMessage(False, 'qt directory could not be found. (see --qtdir).')
            exit(1)

    endMessage(True)

    # remove previous deployment files and copy current binaries

    startMessage('Setting up deployment directory...')

    if os.path.exists(f'deployment'):
        shutil.rmtree(f'deployment')
    
    os.makedirs(f'deployment')

    if os.path.exists(f'bin/{buildArch}/Deploy'):
        shutil.rmtree(f'bin/{buildArch}/Deploy')
    
    os.makedirs(f'bin/{buildArch}/Deploy')

    endMessage(True)

    # create tools folder if it doesn't exist

    startMessage('Setting up tools directory...')

    if not os.path.exists(f'tools'):
        os.makedirs(f'tools')

    endMessage(True)

    if not os.path.isfile('tools/macdeployqtfix/macdeployqtfix.py'):
        if os.path.exists('tools/macdeployqtfix'):
            shutil.rmtree(f'tools/macdeployqtfix')

        startMessage('Cloning macdeployqtfix...')

        resultCode, resultOutput = execute('cd tools;git clone https://github.com/fizzyade/macdeployqtfix.git')

        if resultCode:
            endMessage(False, f'unable to clone macdeployqtfix.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)

    if not os.path.isfile('tools/create-dmg/create-dmg'):
        if os.path.exists('tools/create-dmg'):
            shutil.rmtree(f'tools/create-dmg')

        startMessage('Cloning create-dmg...')

        resultCode, resultOutput = execute('cd tools;git clone https://github.com/andreyvit/create-dmg.git')

        if resultCode:
            endMessage(False, f'unable to clone create-dmg.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)

    if not buildArch=="universal":
        shutil.copytree(f'bin/{buildArch}/{buildType}/{deploymentProject}.app', f'bin/{buildArch}/Deploy/{deploymentProject}.app', symlinks=True)
    else:
        if not os.path.isfile('tools/makeuniversal/makeuniversal'):
            if os.path.exists('tools/makeuniversal'):
                shutil.rmtree(f'tools/makeuniversal')

            startMessage('Cloning makeuniversal...')

            resultCode, resultOutput = execute('cd tools;git clone https://github.com/fizzyade/makeuniversal.git')

            if resultCode:
                endMessage(False, f'unable to clone makeuniversal.\r\n\r\n{resultOutput}\r\n')
                exit(1)

            endMessage(True)

            startMessage('Building makeuniversal...')

            resultCode, resultOutput = execute(f'cd tools/makeuniversal;{qtdir}/bin/qmake;make')

            if resultCode:
                endMessage(False, f'error building makeuniversal.\r\n\r\n{resultOutput}\r\n')
                exit(1)

            endMessage(True)

        startMessage('Running makeuniversal...')

        resultCode, resultOutput = execute(f'tools/makeuniversal/makeuniversal bin/universal/Deploy/{deploymentProject}.app bin/x86_64/{buildType}/{deploymentProject}.app bin/arm64/{buildType}/{deploymentProject}.app')

        if resultCode:
            endMessage(False, f'error building makeuniversal.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)
    
    # run standard qt deployment tool

    startMessage('Running macdeployqt...')

    resultCode, resultOutput = execute(f'{qtdir}/bin/macdeployqt "bin/{buildArch}/Deploy/{deploymentProject}.app" -no-strip')
    resultCode = 0

    if resultCode:
        endMessage(False, f'there was a problem running macdeployqt.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)

    # meanually copy the web engine framework helper as macdeployqt doesn't copy it properrly

    startMessage('Manually copying QtWebEngineProcess helper application...')

    shutil.copytree(f'{qtdir}/lib/QtWebEngineCore.framework/Helpers', f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers', symlinks=True, dirs_exist_ok=True)

    if resultCode:
        endMessage(False, f'there was a problem copying the QtWebEngineProcess helper application.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)

    # remove the sql drivers that we don't use

    startMessage('Removing unwanted qt plugins...')

    if os.path.isfile(f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/PlugIns/sqldrivers/libqsqlodbc.dylib') :
        os.remove(f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/PlugIns/sqldrivers/libqsqlodbc.dylib')

    if os.path.isfile(f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib') :
        os.remove(f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/PlugIns/sqldrivers/libqsqlpsql.dylib')

    endMessage(True)

    # run fixed qt deployment tool

    startMessage('Running macdeployqtfix...')

    sys.path.insert(1, 'tools/macdeployqtfix')

    import macdeployqtfix as fixDeploy

    fixDeploy.GlobalConfig.qtpath = os.path.normpath(f'{qtdir}/bin')
    fixDeploy.GlobalConfig.exepath = f'bin/{buildArch}/Deploy/{deploymentProject}.app'
    fixDeploy.GlobalConfig.logger = logging.getLogger()
    fixDeploy.GlobalConfig.logger.addHandler(logging.NullHandler())

    if not fixDeploy.fix_main_binaries():
        endMessage(False, 'there was a problem running macdeployqtfix.')
        exit(1)

    endMessage(True)

    # sign the application

    startMessage('Signing binaries...')

    resultCode, resultOutput = macSignBinary(f'bin/{buildArch}/Deploy/{deploymentProject}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess', None, args.cert)

    if resultCode:
        endMessage(False, f'there was a problem signing a file QtWebEngineProcess.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    for file in glob.glob(f'bin/{buildArch}/Deploy/{deploymentProject}.app/**/*.framework', recursive=True):
        resultCode, resultOutput = macSignBinary(file, None, args.cert)
        if resultCode:
            endMessage(False, f'there was a problem signing a file ({file}).\r\n\r\n{resultOutput}\r\n')
            exit(1)    

    for file in glob.glob(f'bin/{buildArch}/Deploy/{deploymentProject}.app/**/*.dylib', recursive=True):
        resultCode, resultOutput = macSignBinary(file, None, args.cert)
        if resultCode:
            endMessage(False, f'there was a problem signing a file ({file}).\r\n\r\n{resultOutput}\r\n')
            exit(1)

    resultCode, resultOutput = macSignBinary(f'bin/{buildArch}/Deploy/{deploymentProject}.app', None, args.cert)

    if resultCode:
        endMessage(False, f'there was a problem signing a file ({file}).\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)

    # package the application into a zip file and notarize the application
    
    startMessage('Creating zip archive...')

    resultCode, resultOutput = execute(f'ditto -ck --sequesterRsrc --keepParent "bin/{buildArch}/Deploy/{deploymentProject}.app" "bin/{buildArch}/Deploy/{deploymentProject}.zip"')

    if resultCode:
        endMessage(False, f'there was a problem generating the application zip.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)

    startMessage('Performing notarization of application binary...')

    status = notarizeFile(f'bin/{buildArch}/Deploy/{deploymentProject}.zip', args.appleid, args.password)

    if not status=="success":
        endMessage(False, f'there was a problem notarizing the application ({status}).')
        exit(1)

    endMessage(True)

    startMessage('Stapling notarization ticket to binary...')

    resultCode, resultOutput = execute(f'xcrun stapler staple "bin/{buildArch}/Deploy/{deploymentProject}.app"')

    if resultCode:
        endMessage(False, f'there was a problem stapling the ticket to application.\r\n\r\n{resultOutput}\r\n')
        exit(1)

    endMessage(True)

    outputName = f'bin/{buildArch}/Deploy/{deploymentProject}.app';
    outputType = 'Application'

    # create dmg    

    if (os.path.isfile('assets/dmg_background@1x.tiff') and
        os.path.isfile('assets/dmg_background@2x.tiff') and
        os.path.isfile('assets/dmgbuild_config.py')):
    
        startMessage('Creating installation dmg...')

        resultCode, resultOutput = execute(f'tiffutil -cat "assets/dmg_background@1x.tiff" "assets/dmg_background@2x.tiff" -out "assets/dmg_background.tiff"')

        if resultCode:
            endMessage(False, f'there was a problem creating the combined tiff.\r\n\r\n{resultOutput}\r\n')
            exit(1)
        
        if not os.path.exists('assets/dmgbuild_config.py'):
            endMessage(False, f'the dmgbuild_config.py configuration file could not be found.\r\n\r\n')
            exit(1)

        try:
            resultCode = dmgbuild.build_dmg(volume_name=f'{deploymentProject}', filename=f'./bin/{buildArch}/Deploy/{deploymentProject}.dmg', settings_file='./assets/dmgbuild_config.py', settings={}, defines={'sourceRoot':'./', 'buildArch': buildArch}, lookForHiDPI=True, detach_retries=5)
        
        except:
            endMessage(False, f'there was a problem creating the dmg.\r\n\r\n')
            exit(1)

        if resultCode:
            endMessage(False, f'there was a problem creating the dmg.\r\n\r\n')
            exit(1)

        endMessage(True)

        # sign the dmg and notarize it

        startMessage('Signing dmg...')

        resultCode, resultOutput = macSignBinary(f'./bin/{buildArch}/Deploy/{deploymentProject}.dmg', None, args.cert)

        if resultCode:
            endMessage(False, f'there was a problem signing the dmg.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)

        startMessage('Performing notarization of installation dmg...')

        status = notarizeFile(f'bin/{buildArch}/Deploy/{deploymentProject}.dmg', args.appleid, args.password)

        if not status=="success":
            endMessage(False, f'there was a problem notarizing the dmg ({status}).')
            exit(1)

        endMessage(True)

        startMessage('Stapling notarization ticket to dmg...')

        resultCode, resultOutput = execute(f'xcrun stapler staple "bin/{buildArch}/Deploy/{deploymentProject}.dmg"')

        if resultCode:
            endMessage(False, f'there was a problem stapling the ticket to dmg.\r\n\r\n{resultOutput}\r\n')
            exit(1)

        endMessage(True)

        startMessage('Copying dmg to deployment directory...')

        shutil.copy2(f'bin/{buildArch}/Deploy/{deploymentProject}.dmg', f'deployment/{deploymentProject}.dmg')

        endMessage(True)

        outputName = f'deployment/{deploymentProject}.dmg';
        outputType = 'Disk Image'

    else:
        sys.stdout.write(Style.BRIGHT+Fore.YELLOW+'Skipped DMG creation due to missing files.\r\n')

        
    endTime = time.time()

    # done!

    print(f'\r\n'+Style.BRIGHT+Fore.CYAN+f'Finished! {outputType} at "{outputName}" is '+Fore.GREEN+'ready'+Fore.CYAN+' for distribution.')

def main():

    hostArch = "x86"

    parser = argparse.ArgumentParser(description='Qt Deployment Tool')

    parser.add_argument('--qtdir', type=str, nargs='?', help='path to qt')
    parser.add_argument('--curlbin', type=str, nargs='?', help='path to curl binary')

    if platform.system()=="Darwin":
        status, hostArch = execute(f'arch')

        if not status:
            hostArch = hostArch.strip()

        parser.add_argument('--arch', choices=['x86_64', 'arm64', 'universal'], type=str, default='x64_64', nargs='?', help='architecture type to deploy')
    else:
        parser.add_argument('--arch', choices=['x86', 'x86_64', 'arm64'], type=str, default='x86_64', nargs='?', help='architecture type to deploy')

    parser.add_argument('--type', choices=['release', 'debug'], default='release', type=str, nargs='?', help='type of build to deploy')
    parser.add_argument('--cert', type=str, nargs='?', help='certificate id to sign with')

    if platform.system()=="Linux":
        status, hostArch = execute(f'arch')

        if not status:
            hostArch = hostArch.strip()

        parser.add_argument('--linuxdeployqt', type=str, default=f'tools/linuxdeployqt/linuxdeployqt-6-{hostArch}.AppImage', nargs='?', help='path to linuxdeployqt')
        parser.add_argument('--appimagetool', type=str, default=f'tools/appimagetool/appimagetool-{hostArch}.AppImage', nargs='?', help='path to appimagetool')

    if platform.system()=="Windows":
        parser.add_argument('--timeserver', type=str, default='http://time.certum.pl/', nargs='?', help='time server to use for signing')
        parser.add_argument('--signtool', type=str, nargs='?', default='tools\\smartcardtools\\x64\\ScSignTool.exe', help='path to signing binary')

    if platform.system()=="Darwin":
        parser.add_argument('--appleid', type=str, nargs='?', help='apple id to use for notarization')
        parser.add_argument('--password', type=str, nargs='?', help='password for apple id')

    args = parser.parse_args()

    print(Style.BRIGHT+'Deployment process started at '+str(datetime.datetime.now())+'\r\n')

    startTime = time.time()

    if platform.system()=="Windows":
        deployWindows(args)
    elif platform.system()=="Linux":
        deployLinux(args)
    elif platform.system()=="Darwin":
        deployMacOS(args)
    else:
        print(f'\r\n'+Style.BRIGHT+Fore.RED+f'Unknown platform for deployment. {platform.system()}')
        exit(1)

    endTime = time.time()

    # done!

    print(Style.BRIGHT+f'\r\nTotal time taken to perform deployment was '+timeDelta(endTime-startTime)+'.')

if __name__ == "__main__":
    main()
