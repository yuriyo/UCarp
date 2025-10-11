# UCarp Docker Build for CentOS 7

This directory contains Docker build files for creating UCarp containers based on CentOS 7.

## Files

- **Dockerfile.centos7-base** - Base CentOS 7 image with vault repositories configured
- **Dockerfile.centos7** - Main UCarp build Dockerfile (uses centos7-vault base image)

## Building

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

## Image Sizes

- Base image: ~400MB
- Final UCarp image: ~543MB
