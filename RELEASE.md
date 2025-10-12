# Release Guide

This guide explains how to create a new release of UCarp with pre-built binaries for multiple platforms.

## Quick Release

To create a new release, simply create and push a version tag:

```bash
# Make sure you're on the right branch
git checkout main

# Create a tag (use semantic versioning)
git tag v1.5.3

# Push the tag to trigger the release
git push origin v1.5.3
```

That's it! GitHub Actions will automatically:
1. Build binaries for all supported platforms
2. Create GitHub Release with release notes
3. Upload all binaries with checksums

## What Gets Built

Each release includes **5 pre-built binaries**:

### Modern Systems (Recommended)
- **Debian 12 (x86_64)** - Modern Intel/AMD systems
- **Debian 12 (ARM64)** - Raspberry Pi 4+, AWS Graviton, Apple Silicon
- **Rocky Linux 9 (x86_64)** - RHEL-compatible systems
- **Rocky Linux 9 (ARM64)** - RHEL-compatible ARM systems

### Legacy Systems
- **CentOS 7 (x86_64)** - Maximum compatibility (GLIBC 2.14+)

## Release Process Details

### Automatic (Tag-based)

When you push a tag matching `v*.*.*`:

```bash
git tag v1.5.3
git push origin v1.5.3
```

### Manual (GitHub UI)

If you need to create a test release or re-run a build:

1. Go to GitHub → **Actions** tab
2. Select **"Multi-Platform Release Build"**
3. Click **"Run workflow"**
4. Choose branch (usually `main`)
5. Enter version (e.g., `v1.0.0-test`)
6. Click **"Run workflow"**

## Version Numbering

Use [Semantic Versioning](https://semver.org/):

- **MAJOR** version for incompatible changes (e.g., `v2.0.0`)
- **MINOR** version for new features (e.g., `v1.5.0`)
- **PATCH** version for bug fixes (e.g., `v1.5.3`)

Examples:
```bash
git tag v1.5.3    # Bug fix release
git tag v1.6.0    # New feature release
git tag v2.0.0    # Breaking changes
```

## Pre-Release Testing

Before creating a release tag, test your builds:

1. **Push your code** to a feature branch
2. **Create a Pull Request**
3. **Wait for CI** - The build-test workflow will run automatically
4. **Review results** - All builds must pass

Example:
```bash
git checkout -b feature/new-feature
# Make your changes
git push origin feature/new-feature
# Create PR on GitHub
```

## Checking Release Status

### During Build

1. Go to **Actions** tab
2. Click on the running workflow
3. Monitor each build job:
   - `build-debian` (2 builds: x86_64 + ARM64)
   - `build-rocky9` (2 builds: x86_64 + ARM64)
   - `build-centos7` (1 build: x86_64)
   - `create-release` (creates the release)

### After Completion

1. Go to **Releases** tab
2. Find your version (e.g., `v1.5.3`)
3. Verify all artifacts are present:
   - 5 `.tar.gz` files
   - 5 `.tar.gz.sha256` checksum files
   - Release notes

## Release Artifacts

Each release includes:

```
ucarp-v1.5.3-debian12-linux-amd64.tar.gz      (+ .sha256)
ucarp-v1.5.3-debian12-linux-arm64.tar.gz      (+ .sha256)
ucarp-v1.5.3-rocky9-linux-amd64.tar.gz        (+ .sha256)
ucarp-v1.5.3-rocky9-linux-arm64.tar.gz        (+ .sha256)
ucarp-v1.5.3-centos7-linux-amd64.tar.gz       (+ .sha256 + GLIBC report)
```

## Troubleshooting

### Build Failed

**Check the logs**:
1. Go to Actions → Failed workflow
2. Click on the failed job
3. Expand the failed step
4. Review error messages

**Common issues**:
- **Compilation error**: Fix code and push new commit, then create new tag
- **Dockerfile error**: Fix Dockerfile and push, then create new tag
- **Timeout**: ARM64 builds can be slow; wait or check QEMU setup

### Release Not Created

**Check permissions**:
- Repository must allow GitHub Actions to create releases
- Go to Settings → Actions → General → Workflow permissions
- Ensure "Read and write permissions" is selected

### Wrong Version in Release

Delete the release and tag, then create a new one:

```bash
# Delete tag locally
git tag -d v1.5.3

# Delete tag on remote
git push origin :refs/tags/v1.5.3

# Delete release on GitHub (through UI)
# Then create the correct tag
git tag v1.5.4
git push origin v1.5.4
```

## Testing a Release

After release is created:

```bash
# Download a binary
wget https://github.com/yourusername/UCarp/releases/download/v1.5.3/ucarp-v1.5.3-debian12-linux-amd64.tar.gz

# Verify checksum
wget https://github.com/yourusername/UCarp/releases/download/v1.5.3/ucarp-v1.5.3-debian12-linux-amd64.tar.gz.sha256
sha256sum -c ucarp-v1.5.3-debian12-linux-amd64.tar.gz.sha256

# Extract and test
tar -xzf ucarp-v1.5.3-debian12-linux-amd64.tar.gz
./ucarp-debian12-linux-amd64 --help
```

## Best Practices

1. **Always test before releasing**: Use PRs to test builds
2. **Use semantic versioning**: Clear version numbers help users
3. **Write clear commit messages**: They appear in release context
4. **Update CHANGELOG**: Keep users informed of changes
5. **Test on target platforms**: Download and verify releases work

## Emergency Procedures

### Broken Release

If a release is broken:

1. **Delete the release** (GitHub UI)
2. **Delete the tag**:
   ```bash
   git tag -d v1.5.3
   git push origin :refs/tags/v1.5.3
   ```
3. **Fix the issue**
4. **Create new release** with incremented version:
   ```bash
   git tag v1.5.4
   git push origin v1.5.4
   ```

### Urgent Hotfix

For critical bugs:

1. **Create hotfix branch** from tag:
   ```bash
   git checkout -b hotfix/v1.5.4 v1.5.3
   ```
2. **Fix the issue**
3. **Test thoroughly**
4. **Merge to main**
5. **Create new tag**:
   ```bash
   git tag v1.5.4
   git push origin v1.5.4
   ```

## Additional Resources

- [GitHub Actions Workflows](../.github/workflows/README.md) - Detailed workflow documentation
- [Docker Build Guide](README-Docker.md) - Manual Docker build instructions
- [Semantic Versioning](https://semver.org/) - Version numbering guide

## Questions?

Check the workflow documentation in `.github/workflows/README.md` or review the GitHub Actions logs for detailed build information.
