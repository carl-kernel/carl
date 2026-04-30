// SPDX-License-Identifier: GPL-2.0-only
/*
* carl/init/version.c
*
* (C) Mau 2026
*/

#include <carl/video.h>

#define KERNEL_VERSION "1.1-alpha"
#define KERNEL_CODENAME "Project-Genesis"
#define BUILD_DATE __DATE__ "" __TIME__
#define COMPILR "GCC" __VERSION__

int set_hostname(const char *input) {

    size_t max = sizeof(carl_hostname) - 1;
    size_t len = 0;

    // calculate length first
    while (input[len] != '\0' && len < max) {
        len++;
    }

    // copy in a second step
    for (size_t i = 0; i < len; i++) {
        carl_hostname[i] = input[i];
    }

    carl_hostname[len] = '\0';

    // detect if truncation occurred
    if (input[len] != '\0') {
        video_write("Hostname truncate\n");
        return -1;
    }

    return 0;
}

#define K_NAME       "GENESIS KERNEL CORE"
#define K_MAJOR      "0"
#define K_MINOR      "4"
#define K_PATCH      "12"
#define K_STATUS     "DEVELOPMENT-STABLE"
#define K_CODENAME   "PROMETHEUS-BOUND"
#define K_ARCH       "x86_32-elf"
#define K_LICENSE    "GNU General Public License v2.0 (GPLv2)"

/* Credits and Organization */

#define K_AUTHOR     "Mau"
#define K_ORG        "Genesis Open Source Project"
#define K_WEB        "https://github.com/stevenetworkesp267-sketch/carl.git"

const char *kernel_full_banner = 
    "************************************************************************\n"
    "*                                                                      *\n"
    "*  " K_NAME " - Version " K_MAJOR "." K_MINOR "." K_PATCH " (" K_STATUS ")\n"
    "*  Code Name: " K_CODENAME "                                   *\n"
    "*                                                                      *\n"
    "************************************************************************\n"
    "  System Architecture : " K_ARCH "\n"
    "  Build Environment   : " __VERSION__ "\n"
    "  Timestamp           : " __DATE__ " " __TIME__ "\n"
    "  Legal License       : " K_LICENSE "\n"
    "  Maintainer          : " K_AUTHOR "\n"
    "  Organization        : " K_ORG "\n"
    "  Official Source     : " K_WEB "\n"
    "------------------------------------------------------------------------\n"
    "  WARNING: This is a custom kernel. Use under your own responsibility.  \n"
    "------------------------------------------------------------------------\n";


#define K_NAME       "GENESIS KERNEL CORE"
#define K_MAJOR      "0"
#define K_MINOR      "4"
#define K_PATCH      "12"
#define K_STATUS     "DEVELOPMENT-STABLE"
#define K_CODENAME   "PROMETHEUS-BOUND"
#define K_ARCH       "x86_32-elf"
#define K_LICENSE    "GNU General Public License v2.0 (GPLv2)"

/* Technical information for the system's 'fingerprint' command */

const char *kernel_legal_notice =
"(C) Mau 2026. " K_ORG "\n"
"This is free software; see the source for copying conditions. There is NO\n"
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n";

    /*

* Identification Functions (Kernel API)

* These functions allow other modules to read the version without directly accessing the variables, maintaining encapsulation.

*/

const char *get_kernel_info(int type) {
    switch(type) {
        case 0: return K_NAME;
        case 1: return K_MAJOR "." K_MINOR "." K_PATCH;
        case 2: return K_CODENAME;
        case 3: return K_ARCH;
        case 4: return __DATE__;
        default: return "Unknown";
    }
}

#define KERNEL_VERSION_MAJOR    0
#define KERNEL_VERSION_MINOR    1
#define KERNEL_VERSION_PATCH    0
#define KERNEL_BUILD_NUMBER     1
 
/* Kernel version string format: v0.1.0 */
#define KERNEL_VERSION_STRING   "v0.1.0"
 
/* Build information */
#define KERNEL_BUILD_DATE       __DATE__
#define KERNEL_BUILD_TIME       __TIME__
#define KERNEL_BUILD_COMPILER   "GCC"
 
#ifdef _WIN32
    #define KERNEL_PLATFORM     "Windows"
#elif __APPLE__
    #define KERNEL_PLATFORM     "macOS"
#else
    #define KERNEL_PLATFORM     "Linux"
#endif
 
/* Kernel name and tagline */
#define KERNEL_NAME             "Carl Kernel"
#define KERNEL_TAGLINE          "x86-32 Protected Mode Kernel"
#define KERNEL_AUTHOR           "Mau (Gamer Mauri)"
#define KERNEL_LICENSE          "GPL-2.0-only"
 
/* ============================================================================
 * VERSION INFORMATION STRUCTURE
 * ============================================================================
 */
 
typedef struct {
    int major;              /* Major version number */
    int minor;              /* Minor version number */
    int patch;              /* Patch version number */
    int build;              /* Build number */
    const char *version_str;/* Version string (v0.1.0) */
    const char *name;       /* Kernel name */
    const char *tagline;    /* Tagline/description */
    const char *author;     /* Author name */
    const char *license;    /* License identifier */
    const char *platform;   /* Platform/OS */
    const char *build_date; /* Build date */
    const char *build_time; /* Build time */
    const char *compiler;   /* Compiler used */
} kernel_version_t;
 
/**
 * Global kernel version information
 * Accessible throughout kernel code
 */
static kernel_version_t kernel_version = {
    .major       = KERNEL_VERSION_MAJOR,
    .minor       = KERNEL_VERSION_MINOR,
    .patch       = KERNEL_VERSION_PATCH,
    .build       = KERNEL_BUILD_NUMBER,
    .version_str = KERNEL_VERSION_STRING,
    .name        = KERNEL_NAME,
    .tagline     = KERNEL_TAGLINE,
    .author      = KERNEL_AUTHOR,
    .license     = KERNEL_LICENSE,
    .platform    = KERNEL_PLATFORM,
    .build_date  = KERNEL_BUILD_DATE,
    .build_time  = KERNEL_BUILD_TIME,
    .compiler    = KERNEL_BUILD_COMPILER,
};
 
/* ============================================================================
 * PUBLIC API FUNCTIONS
 * ============================================================================
 */
 
/**
 * kernel_get_version() - Get kernel version number
 * 
 * @return: Version string (e.g., "v0.1.0")
 */
const char *kernel_get_version(void) {
    return kernel_version.version_str;
}
 
/**
 * kernel_get_name() - Get kernel name
 * 
 * @return: Kernel name (e.g., "Carl Kernel")
 */
const char *kernel_get_name(void) {
    return kernel_version.name;
}
 
/**
 * kernel_get_author() - Get kernel author
 * 
 * @return: Author name (e.g., "Mau (Gamer Mauri)")
 */
const char *kernel_get_author(void) {
    return kernel_version.author;
}
 
/**
 * kernel_get_license() - Get kernel license
 * 
 * @return: SPDX license identifier
 */
const char *kernel_get_license(void) {
    return kernel_version.license;
}
 
/**
 * kernel_get_platform() - Get target platform
 * 
 * @return: Platform name (Windows, macOS, Linux)
 */
const char *kernel_get_platform(void) {
    return kernel_version.platform;
}
 
/**
 * kernel_print_version() - Print complete version information
 * 
 * Outputs formatted version banner with all build details
 * Useful during kernel startup for debugging
 */
void kernel_print_version(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                  %s %s                    ║\n",
           kernel_version.name,
           kernel_version.version_str);
    printf("║                %s                ║\n",
           kernel_version.tagline);
    printf("║                   %s                    ║\n",
           kernel_version.license);
    printf("║                 (C) %s                      ║\n",
           kernel_version.author);
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Build Information:\n");
    printf("  Version:    %d.%d.%d (build %d)\n",
           kernel_version.major,
           kernel_version.minor,
           kernel_version.patch,
           kernel_version.build);
    printf("  Date:       %s\n", kernel_version.build_date);
    printf("  Time:       %s\n", kernel_version.build_time);
    printf("  Compiler:   %s\n", kernel_version.compiler);
    printf("  Platform:   %s\n", kernel_version.platform);
    printf("\n");
}
 
/**
 * kernel_version_info() - Get detailed version information structure
 * 
 * @return: Pointer to kernel_version_t structure
 * 
 * Allows access to all version fields at once
 */
const kernel_version_t *kernel_version_info(void) {
    return &kernel_version;
}
 
/* ============================================================================
 * VERSION CHECKING
 * ============================================================================
 */
 
/**
 * kernel_is_version() - Check if kernel version matches specified version
 * 
 * @major: Major version to check
 * @minor: Minor version to check
 * @patch: Patch version to check
 * @return: 1 if version matches, 0 otherwise
 * 
 * Example: kernel_is_version(0, 1, 0) checks if version is v0.1.0
 */
int kernel_is_version(int major, int minor, int patch) {
    return (kernel_version.major == major &&
            kernel_version.minor == minor &&
            kernel_version.patch == patch) ? 1 : 0;
}
 
/**
 * kernel_is_compatible() - Check if kernel version is compatible
 * 
 * @required_major: Required major version
 * @required_minor: Required minor version
 * @return: 1 if current version >= required version, 0 otherwise
 * 
 * Uses semantic versioning:
 * - Major version: Breaking changes
 * - Minor version: New features (backward compatible)
 * - Patch version: Bug fixes
 * 
 * Compatible if:
 * - Major version matches AND
 * - Minor version >= required minor
 */
int kernel_is_compatible(int required_major, int required_minor) {
    if (kernel_version.major != required_major) {
        return 0;  // Major version mismatch = incompatible
    }
    
    return (kernel_version.minor >= required_minor) ? 1 : 0;
}
 
/* ============================================================================
 * FINAL SECTION: Kernel Banner and Credits
 * ============================================================================
 */
 
/**
 * kernel_print_credits() - Display kernel credits
 * 
 * Prints author, contributors, and acknowledgments
 */
void kernel_print_credits(void) {
    printf("\nCarl Kernel Credits:\n");
    printf("─────────────────────────────────────────────────────────────\n");
    printf("Lead Developer:     Mau (Gamer Mauri) <bentosmau@gmail.com>\n");
    printf("License:            GNU General Public License v2.0\n");
    printf("Project Started:    2026\n");
    printf("Repository:         https://github.com/stevenetworkesp267-sketch/carl.git\n");
    printf("\nSpecial Thanks:\n");
    printf("  - Intel x86 Architecture Documentation\n");
    printf("  - OSDEV Community\n");
    printf("  - Free Software Foundation\n");
    printf("─────────────────────────────────────────────────────────────\n\n");
}
 
/**
 * kernel_uname() - Get kernel name in uname format
 * 
 * Returns a string similar to Linux uname output
 * Format: Carl 0.1.0 #1 Sat Dec 14 15:30:45 2026 x86_64
 * 
 * @return: Static string with uname-like format
 * 
 * Note: Format may vary slightly from Linux uname
 */
const char *kernel_uname(void) {
    static char uname_buf[256];
    
    snprintf(uname_buf, sizeof(uname_buf),
             "%s %d.%d.%d #%d %s %s %s",
             kernel_version.name,
             kernel_version.major,
             kernel_version.minor,
             kernel_version.patch,
             kernel_version.build,
             kernel_version.build_date,
             kernel_version.build_time,
             kernel_version.platform);
    
    return uname_buf;
}
 
/**
 * FINAL NOTES:
 * 
 * This version.c file provides:
 * 1. Complete version tracking
 * 2. Build metadata (date, time, compiler)
 * 3. Version validation functions
 * 4. Formatted output for debugging
 * 5. API for querying version info
 * 
 * Usage in kernel:
 *   - Call kernel_print_version() during startup
 *   - Call kernel_is_compatible() in drivers for version checks
 *   - Use kernel_get_version() for logging
 * 
 * Integration with build system:
 *   - Compile with -DKERNEL_VERSION_MAJOR=X
 *   - Or define VERSION_MAJOR in Makefile
 *   - Use git describe for build numbers
 */
