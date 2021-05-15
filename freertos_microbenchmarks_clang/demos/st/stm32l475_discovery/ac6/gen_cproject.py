#!/usr/bin/env python3

import argparse
import os
import sys


#
# Path to the root directory of whole project.
#
root = '${workspace_loc}/..'

#
# Path to our Clang.
#
clang_path = root + '/build/llvm/bin/clang'

#
# Path to the directory of this project.
#
project_dir = os.path.abspath(os.path.dirname(sys.argv[0]))

#
# Project name.
#
project_name = 'freertos_microbenchmarks_clang'


#
# Dict of libraries that need to be linked.
#
libraries = {
    'c': {
        'includes': [
            '${openstm32_compiler_path}/../arm-none-eabi/include',
        ],
        'library_paths': [
            '${openstm32_compiler_path}/../arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard',
            '${openstm32_compiler_path}/../arm-none-eabi/lib',
        ],
        'objects': [
            '${openstm32_compiler_path}/../arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard/crt0.o',
        ],
    },
    'gcc': {
        'library_paths': [
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1/thumb/v7e-m/fpv4-sp/hard',
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1',
            '${openstm32_compiler_path}/../lib/gcc',
        ],
        'objects': [
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1/thumb/v7e-m/fpv4-sp/hard/crti.o',
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1/thumb/v7e-m/fpv4-sp/hard/crtbegin.o',
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1/thumb/v7e-m/fpv4-sp/hard/crtend.o',
            '${openstm32_compiler_path}/../lib/gcc/arm-none-eabi/7.3.1/thumb/v7e-m/fpv4-sp/hard/crtn.o',
        ],
    },
}


#
# Dict of middlewares that are used.
#
middlewares = {
    'BSP': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/B-L475E-IOT01}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/Common}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/hts221}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/lis3mdl}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/lps22hb}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/lsm6dsl}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/vl53l0x}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/BSP/Components/es_wifi}',
        ],
    },
    'CMSIS': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/CMSIS/Include}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/CMSIS/Device/ST/STM32L4xx/Include}',
        ],
    },
    'HAL': {
        'defines': [
            'USE_HAL_DRIVER',
            'STM32L475xx',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/STM32L4xx_HAL_Driver/Inc}',
            '${workspace_loc:/${ProjName}/lib/third_party/mcu_vendor/st/stm32l475_discovery/STM32L4xx_HAL_Driver/Inc/Legacy}',
        ],
    },
    'FreeRTOS': {
        'defines': [
            'MQTTCLIENT_PLATFORM_HEADER=MQTTCMSIS.h',
            'ENABLE_IOT_INFO',
            'ENABLE_IOT_ERROR',
            'SENSOR',
            'RFU',
            'USE_OFFLOAD_SSL',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/config_files}',
            '${workspace_loc:/${ProjName}/lib/aws/include}',
            '${workspace_loc:/${ProjName}/lib/aws/include/private}',
            '${workspace_loc:/${ProjName}/lib/aws/FreeRTOS/portable/GCC/ARM_CM4F}',
        ],
    },
    'jsmn': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/jsmn}',
        ],
    },
    'mbedTLS': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/mbedtls/include}',
        ],
    },
    'pkcs11': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/pkcs11}',
        ],
    },
    'tinycbor': {
        'includes': [
            '${workspace_loc:/${ProjName}/lib/third_party/tinycbor}',
        ],
    },
}


#
# Dict of programs to compile and link.
#
programs = {
    'context-switch': {
        'defines': [
            'CONTEXT_SWITCH_MICRO_BENCHMARK',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/application_code/silhouette_demo/include}',
            '${workspace_loc:/${ProjName}/application_code/st_code}',
            '${workspace_loc:/${ProjName}/application_code/common_demos/include}',
            '${workspace_loc:/${ProjName}/application_code}',
        ],
    },
    'queue': {
        'defines': [
            'UNPRIV_API_BENCHMARK',
            'QUEUE_BENCHMARK',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/application_code/silhouette_demo/include}',
            '${workspace_loc:/${ProjName}/application_code/st_code}',
            '${workspace_loc:/${ProjName}/application_code/common_demos/include}',
            '${workspace_loc:/${ProjName}/application_code}',
        ],
    },
    'stream-buffer': {
        'defines': [
            'UNPRIV_API_BENCHMARK',
            'STREAM_BUFFER_BENCHMARK',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/application_code/silhouette_demo/include}',
            '${workspace_loc:/${ProjName}/application_code/st_code}',
            '${workspace_loc:/${ProjName}/application_code/common_demos/include}',
            '${workspace_loc:/${ProjName}/application_code}',
        ],
    },
    'exception-dispatcher': {
        'defines': [
            'EXCEPTION_NEW_MICRO_BENCHMARK',
        ],
        'includes': [
            '${workspace_loc:/${ProjName}/application_code/silhouette_demo/include}',
            '${workspace_loc:/${ProjName}/application_code/st_code}',
            '${workspace_loc:/${ProjName}/application_code/common_demos/include}',
            '${workspace_loc:/${ProjName}/application_code}',
        ],
    },
}


#
# Dict of configurations.
#
configurations = {
    'baseline': {
    },
}

###############################################################################

#
# Generate and return the cproject header.
#
def gen_header():
    xml =  '<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n'
    xml += '<?fileVersion 4.0.0?>\n'
    xml += '<cproject storage_type_id="org.eclipse.cdt.core.XmlProjectDescriptionStorage">\n'
    return xml


#
# Generate and return the cproject footer.
#
def gen_footer():
    xml =  '  <storageModule moduleId="cdtBuildSystem" version="4.0.0">\n'
    xml += '    <project id="' + project_name + '.fr.ac6.managedbuild.target.gnu.cross.exe" name="Executable" projectType="fr.ac6.managedbuild.target.gnu.cross.exe"/>\n'
    xml += '  </storageModule>\n'
    xml += '  <storageModule moduleId="org.eclipse.cdt.core.LanguageSettingsProviders"/>\n'
    xml += '  <storageModule moduleId="org.eclipse.cdt.make.core.buildtargets"/>\n'
    xml += '  <storageModule moduleId="org.eclipse.cdt.internal.ui.text.commentOwnerProjectMappings"/>\n'
    xml += '</cproject>\n'
    return xml

#
# Generate and return the header of refresh scopes.
#
def gen_refresh_scope_header():
    return '  <storageModule moduleId="refreshScope" versionNumber="2">\n'


#
# Generate and return the refresh scope for a given configuration and program.
#
# @conf: the name of the configuration.
# @program: the name of the program.
#
def gen_refresh_scope_config(conf, program):
    xml =  '    <configuration configurationName="' + conf + '-' + program + '">\n'
    xml += '      <resource resourceType="PROJECT" workspacePath="/' + project_name + '"/>\n'
    xml += '    </configuration>\n'
    return xml


#
# Generate and return the footer of refresh scopes.
#
def gen_refresh_scope_footer():
    return '  </storageModule>\n'


#
# Generate and return the header of scanner configurations.
#
def gen_scanner_header():
    xml =  '  <storageModule moduleId="scannerConfiguration">\n'
    xml += '    <autodiscovery enabled="false" problemReportingEnabled="false" selectedProfileId=""/>\n'
    return xml


#
# Generate and return the scanner configuration for a given configuration and
# program.
#
# @conf: the name of the configuration.
# @program: the name of the program.
#
def gen_scanner_config(conf, program):
    xml =  '    <scannerConfigBuildInfo instanceId="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + conf + '-' + program + ';fr.ac6.managedbuild.config.gnu.cross.exe.release.' + conf + '-' + program + '.;fr.ac6.managedbuild.tool.gnu.cross.c.compiler.' + conf + '-' + program + ';fr.ac6.managedbuild.tool.gnu.cross.c.compiler.input.c.' + conf + '-' + program + '">\n'
    xml += '      <autodiscovery enabled="false" problemReportingEnabled="false" selectedProfileId=""/>\n'
    xml += '    </scannerConfigBuildInfo>\n'
    return xml


#
# Generate and return the footer of scanner configurations.
#
def gen_scanner_footer():
    return '  </storageModule>\n'


#
# Generate and return the header of core settings.
#
def gen_core_settings_header():
    return '  <storageModule moduleId="org.eclipse.cdt.core.settings">\n'


#
# Generate and return the core setting for a given program.
#
# @conf: the name of the configuration to use.
# @program: the name of the program.
#
def gen_core_settings_config(conf, program):
    program_id = conf + '-' + program

    xml =  '    <!-- Configuration of ' + program_id + ' -->\n'
    xml += '    <cconfiguration id="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + program_id + '">\n'
    xml += '      <storageModule buildSystemId="org.eclipse.cdt.managedbuilder.core.configurationDataProvider" id="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + program_id + '" moduleId="org.eclipse.cdt.core.settings" name="' + program_id + '">\n'
    xml += '        <externalSettings/>\n'
    xml += '        <extensions>\n'
    xml += '          <extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>\n'
    xml += '          <extension id="org.eclipse.cdt.core.GASErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
    xml += '          <extension id="org.eclipse.cdt.core.GmakeErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
    xml += '          <extension id="org.eclipse.cdt.core.GLDErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
    xml += '          <extension id="org.eclipse.cdt.core.CWDLocator" point="org.eclipse.cdt.core.ErrorParser"/>\n'
    xml += '          <extension id="org.eclipse.cdt.core.GCCErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>\n'
    xml += '        </extensions>\n'
    xml += '      </storageModule>\n'
    xml += '      <storageModule moduleId="cdtBuildSystem" version="4.0.0">\n'
    xml += '        <configuration artifactExtension="elf" artifactName="${ConfigName}" buildArtefactType="org.eclipse.cdt.build.core.buildArtefactType.exe" buildProperties="org.eclipse.cdt.build.core.buildArtefactType=org.eclipse.cdt.build.core.buildArtefactType.exe,org.eclipse.cdt.build.core.buildType=org.eclipse.cdt.build.core.buildType.release" cleanCommand="rm -rf" description="" id="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + program_id + '" name="' + program_id + '" parent="fr.ac6.managedbuild.config.gnu.cross.exe.release" postannouncebuildStep="Generating binary and Printing size information:" postbuildStep="arm-none-eabi-objcopy -O binary &quot;${BuildArtifactFileBaseName}.elf&quot; &quot;${BuildArtifactFileBaseName}.bin&quot; &amp;&amp; arm-none-eabi-objcopy -O ihex &quot;${BuildArtifactFileBaseName}.elf&quot; &quot;${BuildArtifactFileBaseName}.hex&quot; &amp;&amp; arm-none-eabi-size &quot;${BuildArtifactFileName}&quot;">\n'
    xml += '          <folderInfo id="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + program_id + '." name="/" resourcePath="">\n'
    xml += '            <toolChain id="fr.ac6.managedbuild.toolchain.gnu.cross.exe.release.' + program_id + '" name="Ac6 STM32 MCU GCC" superClass="fr.ac6.managedbuild.toolchain.gnu.cross.exe.release">\n'
    xml += '              <option id="fr.ac6.managedbuild.option.gnu.cross.mcu.' + program_id + '" name="Mcu" superClass="fr.ac6.managedbuild.option.gnu.cross.mcu" value="STM32L475VGTx" valueType="string"/>\n'
    xml += '              <option id="fr.ac6.managedbuild.option.gnu.cross.fpu.' + program_id + '" name="Floating point hardware" superClass="fr.ac6.managedbuild.option.gnu.cross.fpu" value="fr.ac6.managedbuild.option.gnu.cross.fpu.fpv4-sp-d16" valueType="enumerated"/>\n'
    xml += '              <option id="fr.ac6.managedbuild.option.gnu.cross.floatabi.' + program_id + '" name="Floating-point ABI" superClass="fr.ac6.managedbuild.option.gnu.cross.floatabi" value="fr.ac6.managedbuild.option.gnu.cross.floatabi.hard" valueType="enumerated"/>\n'
    xml += '              <option id="fr.ac6.managedbuild.option.gnu.cross.board.' + program_id + '" name="Board" superClass="fr.ac6.managedbuild.option.gnu.cross.board" value="STM32L475VG" valueType="string"/>\n'
    xml += '              <option id="fr.ac6.managedbuild.option.gnu.cross.prefix.' + program_id + '" name="Prefix" superClass="fr.ac6.managedbuild.option.gnu.cross.prefix" value="" valueType="string"/>\n'
    xml += '              <targetPlatform archList="all" binaryParser="org.eclipse.cdt.core.ELF" id="fr.ac6.managedbuild.targetPlatform.gnu.cross.' + program_id + '" isAbstract="false" osList="all" superClass="fr.ac6.managedbuild.targetPlatform.gnu.cross"/>\n'
    xml += '              <builder buildPath="${workspace_loc:/' + project_name + '}/Release" id="fr.ac6.managedbuild.builder.gnu.cross.' + program_id + '" keepEnvironmentInBuildfile="false" managedBuildOn="true" name="Gnu Make Builder" parallelBuildOn="true" parallelizationNumber="optimal" superClass="fr.ac6.managedbuild.builder.gnu.cross"/>\n'
    ###########################################################################
    # Set up C compiler
    ###########################################################################
    xml += '              <tool command="' + clang_path + '" id="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.' + program_id + '" name="MCU GCC Compiler" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.compiler">\n'
    xml += '                <option id="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.fdata.' + program_id + '" name="Place the data in their own section (-fdata-sections)" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.fdata" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
    xml += '                <option id="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.ffunction.' + program_id + '" name="Place the function in their own section (-ffunction-sections)" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.ffunction" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
    xml += '                <option id="fr.ac6.managedbuild.gnu.c.compiler.option.optimization.level.' + program_id + '" name="Optimization Level" superClass="fr.ac6.managedbuild.gnu.c.compiler.option.optimization.level" useByScannerDiscovery="false" value="fr.ac6.managedbuild.gnu.c.optimization.level.most" valueType="enumerated"/>\n'
    xml += '                <option defaultValue="gnu.c.debugging.level.none" id="gnu.c.compiler.option.debugging.level.' + program_id + '" name="Debug Level" superClass="gnu.c.compiler.option.debugging.level" useByScannerDiscovery="false" valueType="enumerated"/>\n'
    # Add macro definitions
    xml += '                <option id="gnu.c.compiler.option.preprocessor.def.symbols.' + program_id + '" name="Defined symbols (-D)" superClass="gnu.c.compiler.option.preprocessor.def.symbols" useByScannerDiscovery="false" valueType="definedSymbols">\n'
    for library in libraries:
        if 'defines' in libraries[library]:
            for define in libraries[library]['defines']:
                define = define.replace('"', '\&quot;')
                xml += '                  <listOptionValue builtIn="false" value="' + define + '"/>\n'
    for middleware in middlewares:
        if 'defines' in middlewares[middleware]:
            for define in middlewares[middleware]['defines']:
                define = define.replace('"', '\&quot;')
                xml += '                  <listOptionValue builtIn="false" value="' + define + '"/>\n'
    if 'defines' in configurations[conf]:
        for define in configurations[conf]['defines']:
            define = define.replace('"', '\&quot;')
            xml += '                  <listOptionValue builtIn="false" value="' + define + '"/>\n'
    if 'defines' in programs[program]:
        for define in programs[program]['defines']:
            define = define.replace('"', '\&quot;')
            xml += '                  <listOptionValue builtIn="false" value="' + define + '"/>\n'
    xml += '                </option>\n'
    # Add include paths
    xml += '                <option id="gnu.c.compiler.option.include.paths.' + program_id + '" name="Include paths (-I)" superClass="gnu.c.compiler.option.include.paths" useByScannerDiscovery="false" valueType="includePath">\n'
    for library in libraries:
        if 'includes' in libraries[library]:
            for include in libraries[library]['includes']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    for middleware in middlewares:
        if 'includes' in middlewares[middleware]:
            for include in middlewares[middleware]['includes']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    if 'includes' in configurations[conf]:
        for include in configurations[conf]["includes"]:
            xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    if 'includes' in programs[program]:
        for include in programs[program]['includes']:
            xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    xml += '                </option>\n'
    # Add other C flags
    xml += '                <option id="fr.ac6.managedbuid.gnu.c.compiler.option.misc.other.' + program_id + '" name="Other flags" superClass="fr.ac6.managedbuid.gnu.c.compiler.option.misc.other" useByScannerDiscovery="false" value="--target=arm-none-eabi'
    for library in libraries:
        if 'cflags' in libraries[library]:
            for cflag in libraries[library]['cflags']:
                xml += ' ' + cflag
    for middleware in middlewares:
        if 'cflags' in middlewares[middleware]:
            for cflag in middlewares[middleware]['cflags']:
                xml += ' ' + cflag
    if 'cflags' in configurations[conf]:
        for cflag in configurations[conf]['cflags']:
            xml += ' ' + cflag
    if 'cflags' in programs[program]:
        for cflag in programs[program]['cflags']:
            xml += ' ' + cflag
    xml += '" valueType="string"/>\n'
    xml += '                <option id="gnu.c.compiler.option.optimization.flags.' + program_id + '" name="Other optimization flags" superClass="gnu.c.compiler.option.optimization.flags" useByScannerDiscovery="false" value="-fomit-frame-pointer" valueType="string"/>\n'
    xml += '                <option id="gnu.c.compiler.option.dialect.std.' + program_id + '" name="Language standard" superClass="gnu.c.compiler.option.dialect.std" useByScannerDiscovery="false" value="gnu.c.compiler.dialect.default" valueType="enumerated"/>\n'
    xml += '                <option id="gnu.c.compiler.option.misc.verbose.' + program_id + '" name="Verbose (-v)" superClass="gnu.c.compiler.option.misc.verbose" useByScannerDiscovery="false" value="false" valueType="boolean"/>\n'
    xml += '                <option id="gnu.c.compiler.option.include.files.' + program_id + '" name="Include files (-include)" superClass="gnu.c.compiler.option.include.files" useByScannerDiscovery="false"/>\n'
    xml += '                <inputType id="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.input.c.' + program_id + '" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.input.c"/>\n'
    xml += '                <inputType id="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.input.s.' + program_id + '" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.compiler.input.s"/>\n'
    xml += '              </tool>\n'
    ###########################################################################
    # Set up C++ compiler
    ###########################################################################
    xml += '              <tool id="fr.ac6.managedbuild.tool.gnu.cross.cpp.compiler.' + program_id + '" name="MCU G++ Compiler" superClass="fr.ac6.managedbuild.tool.gnu.cross.cpp.compiler">\n'
    xml += '                <option id="fr.ac6.managedbuild.gnu.cpp.compiler.option.optimization.level.' + program_id + '" name="Optimization Level" superClass="fr.ac6.managedbuild.gnu.cpp.compiler.option.optimization.level" useByScannerDiscovery="false" value="fr.ac6.managedbuild.gnu.cpp.optimization.level.most" valueType="enumerated"/>\n'
    xml += '                <option defaultValue="gnu.cpp.compiler.debugging.level.none" id="gnu.cpp.compiler.option.debugging.level.' + program_id + '" name="Debug Level" superClass="gnu.cpp.compiler.option.debugging.level" useByScannerDiscovery="false" valueType="enumerated"/>\n'
    xml += '              </tool>\n'
    ###########################################################################
    # Set up C linker
    ###########################################################################
    xml += '              <tool command="' + clang_path + '" id="fr.ac6.managedbuild.tool.gnu.cross.c.linker.' + program_id + '" name="MCU GCC Linker" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.linker">\n'
    # Add linker script
    xml += '                <option id="fr.ac6.managedbuild.tool.gnu.cross.c.linker.script.' + program_id + '" name="Linker Script (-T)" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.linker.script" useByScannerDiscovery="false" value="${ProjDirPath}/STM32L475VGTx_FLASH.ld" valueType="string"/>\n'
    # Add linked libraries
    xml += '                <option id="gnu.c.link.option.libs.' + program_id + '" name="Libraries (-l)" superClass="gnu.c.link.option.libs" useByScannerDiscovery="false" valueType="libs">\n'
    for library in libraries:
        xml += '                  <listOptionValue builtIn="false" value="' + library + '"/>\n'
    xml += '                </option>\n'
    # Add other linker flags
    xml += '                <option id="gnu.c.link.option.ldflags.' + program_id + '" name="Linker flags" superClass="gnu.c.link.option.ldflags" useByScannerDiscovery="false" value="--target=arm-none-eabi -fuse-ld=lld'
    for library in libraries:
        if 'ldflags' in libraries[library]:
            for ldflag in libraries[library]['ldflags']:
                xml += ' ' + ldflag
    for middleware in middlewares:
        if 'ldflags' in middlewares[middleware]:
            for ldflag in middlewares[middleware]['ldflags']:
                xml += ' ' + ldflag
    if 'ldflags' in configurations[conf]:
        for ldflag in configurations[conf]['ldflags']:
            xml += ' ' + ldflag
    if 'ldflags' in programs[program]:
        for ldflag in programs[program]['ldflags']:
            xml += ' ' + ldflag
    xml += '" valueType="string"/>\n'
    # Add library search paths
    xml += '                <option id="gnu.c.link.option.paths.' + program_id + '" name="Library search path (-L)" superClass="gnu.c.link.option.paths" useByScannerDiscovery="false" valueType="libPaths">\n'
    for library in libraries:
        if 'library_paths' in libraries[library]:
            for library_path in libraries[library]['library_paths']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + library_path + '&quot;"/>\n'
    xml += '                </option>\n'
    # Add other objects
    xml += '                <option id="gnu.c.link.option.userobjs.' + program_id + '" name="Other objects" superClass="gnu.c.link.option.userobjs" useByScannerDiscovery="false" valueType="userObjs">\n'
    for library in libraries:
        if 'objects' in libraries[library]:
            for obj in libraries[library]['objects']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + obj + '&quot;"/>\n'
    xml += '                </option>\n'
    xml += '                <option id="fr.ac6.managedbuild.tool.gnu.cross.c.linker.gcsections.' + program_id + '" name="Discard unused sections (-Wl,--gc-sections)" superClass="fr.ac6.managedbuild.tool.gnu.cross.c.linker.gcsections" useByScannerDiscovery="false" value="true" valueType="boolean"/>\n'
    xml += '                <inputType id="cdt.managedbuild.tool.gnu.c.linker.input.' + program_id + '" superClass="cdt.managedbuild.tool.gnu.c.linker.input">\n'
    xml += '                  <additionalInput kind="additionalinputdependency" paths="$(USER_OBJS)"/>\n'
    xml += '                  <additionalInput kind="additionalinput" paths="$(LIBS)"/>\n'
    xml += '                </inputType>\n'
    xml += '              </tool>\n'
    ###########################################################################
    # Set up C++ linker
    ###########################################################################
    xml += '              <tool id="fr.ac6.managedbuild.tool.gnu.cross.cpp.linker.' + program_id + '" name="MCU G++ Linker" superClass="fr.ac6.managedbuild.tool.gnu.cross.cpp.linker"/>\n'
    ###########################################################################
    # Set up archiver
    ###########################################################################
    xml += '              <tool id="fr.ac6.managedbuild.tool.gnu.archiver.' + program_id + '" name="MCU GCC Archiver" superClass="fr.ac6.managedbuild.tool.gnu.archiver"/>\n'
    ###########################################################################
    # Set up assembler
    ###########################################################################
    xml += '              <tool command="' + clang_path + '" id="fr.ac6.managedbuild.tool.gnu.cross.assembler.exe.release.' + program_id + '" name="MCU GCC Assembler" superClass="fr.ac6.managedbuild.tool.gnu.cross.assembler.exe.release">\n'
    # Add include paths
    xml += '                <option id="gnu.both.asm.option.include.paths.' + program_id + '" name="Include paths (-I)" superClass="gnu.both.asm.option.include.paths" useByScannerDiscovery="false" valueType="includePath">\n'
    for library in libraries:
        if 'includes' in libraries[library]:
            for include in libraries[library]['includes']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    for middleware in middlewares:
        if 'includes' in middlewares[middleware]:
            for include in middlewares[middleware]['includes']:
                xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    if 'includes' in configurations[conf]:
        for include in configurations[conf]["includes"]:
            xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    if 'includes' in programs[program]:
        for include in programs[program]['includes']:
            xml += '                  <listOptionValue builtIn="false" value="&quot;' + include + '&quot;"/>\n'
    xml += '                </option>\n'
    xml += '                <option id="gnu.both.asm.option.flags.' + program_id + '" name="Assembler flags" superClass="gnu.both.asm.option.flags" useByScannerDiscovery="false" value="--target=arm-none-eabi -c" valueType="string"/>\n'
    xml += '                <inputType id="cdt.managedbuild.tool.gnu.assembler.input.' + program_id + '" superClass="cdt.managedbuild.tool.gnu.assembler.input"/>\n'
    xml += '                <inputType id="fr.ac6.managedbuild.tool.gnu.cross.assembler.input.' + program_id + '" superClass="fr.ac6.managedbuild.tool.gnu.cross.assembler.input"/>\n'
    xml += '              </tool>\n'
    xml += '            </toolChain>\n'
    xml += '          </folderInfo>\n'
    ###########################################################################
    # Set up source entries
    ###########################################################################
    xml += '          <sourceEntries>\n'
    if 'directories' in programs[program]:
        for directory in programs[program]['directories']:
            xml += '            <entry excluding="' + programs[program]['directories'][directory] + '" flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="' + directory + '"/>\n'
    for middleware in middlewares:
        if 'directories' in middlewares[middleware]:
            for directory in middlewares[middleware]['directories']:
                xml += '            <entry excluding="' + middlewares[middleware]['directories'][directory] + '" flags="VALUE_WORKSPACE_PATH|RESOLVED" kind="sourcePath" name="' + directory + '"/>\n'
    xml += '          </sourceEntries>\n'
    xml += '        </configuration>\n'
    xml += '      </storageModule>\n'
    xml += '      <storageModule moduleId="org.eclipse.cdt.core.externalSettings"/>\n'
    xml += '    </cconfiguration>\n'

    return xml


#
# Generate and return the footer of core settings.
#
def gen_core_settings_footer():
    return '  </storageModule>\n'


#
# Generate and return the whole cproject file content for all configurations.
#
def gen_cproject():
    xml =  gen_header()

    # Generate core settings for each pair of configuration and program
    xml += gen_core_settings_header()
    for conf in configurations:
        for program in sorted(programs.keys()):
            xml += gen_core_settings_config(conf, program)
    xml += gen_core_settings_footer()

    # Generate refresh scope for each pair of configuration and program
    xml += gen_refresh_scope_header()
    for conf in configurations:
        for program in sorted(programs.keys()):
            xml += gen_refresh_scope_config(conf, program)
    xml += gen_refresh_scope_footer()

    # Generate scanner configuration for each program
    xml += gen_scanner_header()
    for conf in configurations:
        for program in sorted(programs.keys()):
            xml += gen_scanner_config(conf, program)
    xml += gen_scanner_footer()

    xml += gen_footer()

    return xml


#
# Generate and return the file content of language.settings.xml which disables
# discovering compiler's built-in language settings.
#
def gen_language_settings():
    xml  = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>\n'
    xml += '<project>\n'
    for conf in configurations:
        for program in sorted(programs.keys()):
            xml += '  <configuration id="fr.ac6.managedbuild.config.gnu.cross.exe.release.' + conf + '-' + program + '" name="' + conf + '-' + program + '">\n'
            xml += '    <extension point="org.eclipse.cdt.core.LanguageSettingsProvider">\n'
            xml += '      <provider copy-of="extension" id="org.eclipse.cdt.ui.UserLanguageSettingsProvider"/>\n'
            xml += '      <provider-reference id="org.eclipse.cdt.core.ReferencedProjectsLanguageSettingsProvider" ref="shared-provider"/>\n'
            xml += '      <provider-reference id="org.eclipse.cdt.managedbuilder.core.MBSLanguageSettingsProvider" ref="shared-provider"/>\n'
            xml += '    </extension>\n'
            xml += '  </configuration>\n'
    xml += '</project>\n'

    return xml


#
# The main function.
#
def main():
    # Generate a .cproject file for all configurations
    conf_filename = project_dir + '/.cproject'
    xml = gen_cproject()
    with open(conf_filename, 'w') as f:
        f.write(xml)

    # In addition, also generate language.settings.xml that disable discovering
    # compiler's built-in language settings
    settings_dir = project_dir + '/.settings'
    if not os.path.isdir(settings_dir):
        os.mkdir(settings_dir)
    with open(settings_dir + '/language.settings.xml', 'w') as f:
        f.write(gen_language_settings())


if __name__ == '__main__':
    main()
