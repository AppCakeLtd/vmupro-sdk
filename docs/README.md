# VMU Pro LUA SDK Documentation

Welcome to the VMU Pro LUA SDK documentation. This comprehensive guide will help you build amazing applications for the VMU Pro using LUA.

## What is the VMU Pro LUA SDK?

The VMU Pro LUA SDK is a powerful, sandboxed scripting environment that allows developers to create applications for the VMU Pro device using the LUA programming language. The SDK provides secure access to graphics, audio, input, and file system APIs while maintaining system stability and security.

## Key Features

- **Sandboxed Execution**: Your LUA scripts run in a secure, isolated environment
- **Rich APIs**: Access to graphics, audio, input, and file system functionality
- **Memory Safety**: Controlled heap allocation and memory isolation
- **Easy Development**: Simple tooling for packaging and deploying applications
- **Cross-Platform**: Develop on any platform that supports Python and LUA

## Architecture Overview

The VMU Pro LUA SDK uses a dual execution mode architecture:

- **VMUPRO_APP_MODE_NATIVE**: Traditional ELF-based C applications
- **VMUPRO_APP_MODE_LUA**: New LUA-based applications with sandboxed execution

Your LUA applications run in a secure VM with restricted global functions and whitelisted API access only.

## Quick Start

1. **Setup**: Install the required tools and dependencies
2. **Hello World**: Create your first LUA application
3. **Package**: Use the packer tool to create a .vmupack file
4. **Deploy**: Send your application to the VMU Pro device

## Security Model

The LUA SDK implements a comprehensive security model:

- Sandboxed LUA VM with restricted global functions
- Memory isolation and controlled heap allocation
- File system access restricted to `/sdcard` only
- No raw pointer access from LUA
- Parameter validation on all API calls

## Getting Help

- Check the [Getting Started](getting-started.md) guide
- Browse the [API Reference](api/graphics.md) documentation
- Look at the [Examples](examples/hello-world.md) for inspiration
- Review the [Troubleshooting](advanced/troubleshooting.md) guide

Ready to start building? Head over to the [Getting Started](getting-started.md) guide!