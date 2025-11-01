# UCarp Docker Build

This directory contains Docker build files for creating UCarp containers.

## Available Images

### 1. Debian 12 (Recommended for modern systems)
- **File**: `Dockerfile.debian`
- **Base**: Debian 12 (Bookworm)
- **Architecture**: Native ARM64 and x86_64 support
- **Size**: ~209MB
- **Best for**: Modern deployments, smaller images, native multi-arch

```bash
docker build -f Dockerfile.debian -t ucarp:debian .
```

### 2. Rocky Linux 9 (Alternative RHEL-based option)
- **File**: `Dockerfile.rocky9`
- **Base**: Rocky Linux 9
- **Architecture**: Native ARM64 and x86_64 support
- **Size**: TBD
- **Best for**: RHEL-compatible deployments, enterprise environments

```bash
docker build -f Dockerfile.rocky9 -t ucarp:rocky9 .
```

### 3. CentOS 7 (For legacy compatibility)
- **Files**: `Dockerfile.centos7-base` + `Dockerfile.centos7`
- **Base**: CentOS 7.9.2009
- **Architecture**: x86_64 only (uses emulation on ARM64)
- **Size**: ~574MB
- **Best for**: Legacy systems requiring CentOS 7

See below for CentOS 7 build instructions.

## Quick Start (Debian)

Build and run with Debian (recommended):

```bash
docker build -f Dockerfile.debian -t ucarp:debian .
docker run --rm ucarp:debian --help
```

## CentOS 7 Build (Legacy)

### 1. Build the base image (one time only)

The base image configures CentOS 7 vault repositories and updates the system. Build it once:

```bash
docker build -f Dockerfile.centos7-base -t centos7-vault:latest .
```

This creates a clean base image (`centos7-vault:latest`) that can be reused for multiple builds.

### 2. Build UCarp

Once the base image is built, you can build UCarp using the clean Dockerfile:

```bash
docker build -f Dockerfile.centos7 -t ucarp:centos7 .
```

## Why Two Dockerfiles?

The two-stage approach provides several benefits:

1. **Cleaner main Dockerfile** - No repository configuration clutter
2. **Faster rebuilds** - Base image is cached and reused
3. **Reusability** - The base image can be used for other CentOS 7 projects
4. **Easier maintenance** - Repository configuration is centralized

## Architecture Notes

These builds use `--platform=linux/amd64` to ensure compatibility with CentOS 7 vault repositories, which only have x86_64 packages available. The images will run using emulation on ARM64 systems (like Apple Silicon Macs).

## Testing

Run the built image to verify it works:

```bash
docker run --rm ucarp:centos7 --help
```

## Image Comparison

| Distribution | Architecture | Size | Build Time | Use Case |
|--------------|-------------|------|------------|----------|
| **Debian 12** | ARM64/x86_64 native | ~209MB | Fast | Recommended for modern deployments |
| CentOS 7 | x86_64 only | ~574MB | Slower | Legacy systems only |
| Rocky Linux 9 | ARM64/x86_64 native | TBD | TBD | Alternative RHEL-based option |

## Why Debian?

- **Smaller images**: 209MB vs 574MB (64% smaller)
- **Native multi-arch**: Builds natively on both ARM64 and x86_64
- **Modern**: Up-to-date packages and security updates
- **Faster builds**: No emulation needed on ARM64 systems
- **Active support**: Debian 12 is actively maintained

## Testing

Run the built image:

```bash
docker run --rm ucarp:debian --help
```

Check the architecture:

```bash
docker image inspect ucarp:debian --format '{{.Os}}/{{.Architecture}}'
```

---

## Legacy: CentOS 7 Build Instructions

For systems that specifically require CentOS 7:

### 1. Build the base image (one time only)
