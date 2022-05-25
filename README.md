# Feedback Delay Network Reverberator implementation in JUCE
![GitHub Workflow Status](https://img.shields.io/github/workflow/status/VoggLyster/Reverberator/Build%20plugin)
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/VoggLyster/Reverberator)

## The plugin

Following FDN architecture has been implemented as a VST3 plugin in JUCE

![FDN architecture diagram](./images/FDN.png)
The plugin is configured for 16 delay lines with graphic equalizers, pre-delay, and modulating delay-lines.

With scrollable UI:

![FDN GUI](./images/GUI.png)

---
## Compiling the plugin
The plugin is created with JUCE version 6.1.2. 
To create build configurations run the Projucer.

Current configurations exist for VisualStudio 2019 and LinuxMakefile.
