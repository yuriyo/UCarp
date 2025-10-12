# GitHub Actions Workflows

This directory contains automated workflows for building and releasing UCarp across multiple platforms.

## Workflows

### 1. `release.yml` - Multi-Platform Release Build

**Trigger**: 
- Push tags matching `v*.*.*` (e.g., `v1.5.3`)
- Manual workflow dispatch with custom version

**What it does**:
- Builds UCarp for **3 distributions** and **5 total binaries**:
  - **Debian 12**: x86_64 and ARM64
  - **Rocky Linux 9**: x86_64 and ARM64
  - **CentOS 7**: x86_64 only (legacy)
- Extracts binaries from Docker containers
- Creates tarballs with SHA256 checksums
- Generates GLIBC compatibility report for CentOS 7 build
- Creates a GitHub Release with all artifacts
- Includes detailed release notes

**Artifacts produced**:
```
ucarp-v1.0.0-debian12-linux-amd64.tar.gz
ucarp-v1.0.0-debian12-linux-amd64.tar.gz.sha256
ucarp-v1.0.0-debian12-linux-arm64.tar.gz
ucarp-v1.0.0-debian12-linux-arm64.tar.gz.sha256
ucarp-v1.0.0-rocky9-linux-amd64.tar.gz
ucarp-v1.0.0-rocky9-linux-amd64.tar.gz.sha256
ucarp-v1.0.0-rocky9-linux-arm64.tar.gz
ucarp-v1.0.0-rocky9-linux-arm64.tar.gz.sha256
ucarp-v1.0.0-centos7-linux-amd64.tar.gz
ucarp-v1.0.0-centos7-linux-amd64.tar.gz.sha256
```

### 2. `build-test.yml` - Build Testing

**Trigger**:
- Pull requests to main/master/develop branches
- Pushes to main/master/develop branches

**What it does**:
- Tests builds for all Dockerfiles
- Verifies binaries run correctly (`--help` test)
- Checks GLIBC compatibility for CentOS 7
- Runs in parallel for faster feedback

**Build matrix**:
- Debian 12: linux/amd64, linux/arm64
- Rocky Linux 9: linux/amd64, linux/arm64
- CentOS 7: linux/amd64

## Usage

### Creating a Release

#### Option 1: Git Tag (Recommended)

```bash
# Create and push a version tag
git tag v1.5.3
git push origin v1.5.3
```

This will automatically trigger the release workflow.

#### Option 2: Manual Dispatch

1. Go to **Actions** tab in GitHub
2. Select **"Multi-Platform Release Build"**
3. Click **"Run workflow"**
4. Enter version (e.g., `v1.0.0-test`)
5. Click **"Run workflow"**

### Testing Before Release

Push your changes to a branch and create a PR. The `build-test.yml` workflow will automatically:
- Build all platform variants
- Test each binary
- Report any build failures

## Build Details

### Platforms & Compatibility

| Distribution | Architectures | GLIBC | Use Case |
|--------------|---------------|-------|----------|
| **Debian 12** | x86_64, ARM64 | 2.36+ | Modern systems, cloud, recommended |
| **Rocky 9** | x86_64, ARM64 | 2.34+ | RHEL-compatible, enterprise |
| **CentOS 7** | x86_64 only | 2.14+ | Legacy systems, maximum compatibility |

### Build Process

Each build:
1. Uses Docker multi-stage builds
2. Compiles from source using autotools
3. Stops at the `build` stage (no runtime layer needed for binary extraction)
4. Extracts the static binary from `/out/usr/local/sbin/ucarp`
5. Creates compressed tarball with checksums
6. Uploads to GitHub Release

### GLIBC Compatibility

The CentOS 7 build is specifically designed for maximum backward compatibility:
- Built with GLIBC 2.17 (CentOS 7)
- Only requires GLIBC 2.14+ to run
- Compatible with most Linux systems from 2011 onwards

The workflow automatically includes a GLIBC compatibility report in the CentOS 7 tarball.

## Requirements

- Repository must have release/tag permissions
- GitHub Actions must be enabled
- Docker buildx support (provided by default)
- QEMU for cross-platform builds (automatically set up)

## Troubleshooting

### Build Failures

**CentOS 7 builds fail**:
- Ensure `Dockerfile.centos7-base` is present
- Check that vault.centos.org is accessible
- The base image must be built first in the workflow

**ARM64 builds timeout**:
- ARM64 builds use QEMU emulation and may be slower
- Adjust timeout if needed in workflow

**Extraction fails**:
- Verify the binary path is `/out/usr/local/sbin/ucarp`
- Check that `--target=build` stops at the correct stage

### Release Not Created

- Verify repository has `contents: write` permission
- Check that `GITHUB_TOKEN` has access
- Ensure tag format matches `v*.*.*`

## Advanced Configuration

### Modifying Build Matrix

Edit the `matrix.platform` section in `release.yml`:

```yaml
strategy:
  matrix:
    platform:
      - linux/amd64
      - linux/arm64
      # - linux/arm/v7  # Add more platforms
```

### Custom Release Notes

Edit the `Generate release notes` step in the `create-release` job.

### Additional Distributions

To add more distributions:
1. Create a new job similar to `build-debian`
2. Update the Dockerfile reference
3. Add the job to `needs:` in `create-release`

## Security

- Binaries are built in isolated Docker containers
- All artifacts include SHA256 checksums for verification
- No credentials or secrets are required
- Uses official GitHub Actions only

## Performance

Typical build times (on GitHub Actions):
- Debian 12: ~3-5 minutes per platform
- Rocky Linux 9: ~3-5 minutes per platform
- CentOS 7: ~5-7 minutes (includes base image build)

Total release time: ~15-20 minutes for all builds
