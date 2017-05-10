/**
 * @file Version.h
 * Definition and implementation of the Version class.
 */

#ifndef HEADER_UGP3_VERSION
/** Defines that this file has been included */
#define HEADER_UGP3_VERSION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "IString.h"
#include "Convert.h"

/**
 * ugp3 namespace
 */
namespace ugp3
{

/**
 * @class Version
 * Keeps information about the version of the application and provides methods to access them.
 */
class Version : public IString
{
private:
        // Version and revision information
	unsigned int majorV, minorV, revision;
        // Stage of the compilation of this version
	std::string stage;
        // Name of the actual release of the application
	std::string releaseName;

public:
        /** 
         * Constructor of the class. Creates a new version object with the specified parameters
         * @param major Major number of the version
         * @param minor Minor number of the version
         * @param revision Revision number of this version
         * @param stage of the compilation of this version
         * @param releaseName Name of this release
         */
	Version(unsigned int major, unsigned int minor, unsigned int revision, 
		const std::string& stage = "release",
		const std::string& releaseName = "");

        /** 
         * Returns the minor number of this version
         * @param int Minor number of the version
         */
	int getMinor() const;
        /** 
         * Returns the mayor number of this version
         * @param int Mayor number of the version
         */
	int getMajor() const;
        /** 
         * Returns the revision number of this compilation
         * @param int Revision number of this compilation
         */
	int getRevision() const;
	
        /** 
         * Returns the operating system where the application is running
         * @param string Operating system where the application is running
         */
	const std::string getOperatingSystem() const;
        /** 
         * Returns the platform where the application is running
         * @param string Platform where the application is running
         */
	const std::string getHardwarePlatform() const;

        /** 
         * Returns the version, revision and stage of this compilation
         * @param string Version, revision and stage of this compilation
         */
	virtual const std::string toString() const;

        /** 
         * Destructor of the class. For clean if necessary.
         */
	virtual ~Version();
};

inline Version::Version(
    unsigned int major, 
    unsigned int minor, 
    unsigned int revision, 
    const std::string& stage,
    const std::string& releaseName)
	: majorV(major), minorV(minor), revision(revision), stage(stage), releaseName(releaseName)
{ }

inline Version::~Version()
{ }

inline int Version::getMinor() const
{
	return this->minorV;
}

inline int Version::getMajor() const
{
	return this->majorV;
}

inline int Version::getRevision() const
{
	return this->revision;
}

inline const std::string Version::getOperatingSystem() const
{
#if defined(_win32) || defined(_WIN32) || defined(__win32) || defined(__WIN32) || defined(_win32_) || defined(_WIN32_) || defined(__win32__) || defined(__WIN32__)
	return "win32";
#elif defined(_gnu_linux) || defined(_GNU_LINUX) || defined(__gnu_linux) || defined(__GNU_LINUX) || defined(_gnu_linux_) || defined(_GNU_LINUX_) || defined(__gnu_linux__) || defined(__GNU_LINUX__)
	return "gnu-linux";
#elif defined(_linux) || defined(_LINUX) || defined(__linux) || defined(__LINUX) || defined(_linux_) || defined(_LINUX_) || defined(__linux__) || defined(__LINUX__)
	return "linux";
#elif defined(_unix) || defined(_UNIX) || defined(__unix) || defined(__UNIX) || defined(_unix_) || defined(_UNIX_) || defined(__unix__) || defined(__UNIX__)
	return "unix";
#else
    return "unknown-os";
#endif
}

inline const std::string Version::getHardwarePlatform() const
{
#if defined(__AMD64__) || defined(__AMD64) || defined(__amd64__) || defined(__amd64)
	return "amd64";
#elif defined(__IA64__) || defined(__IA64) || defined(__ia64__) || defined(__ia64)
	return "ia64";
#elif defined(X86_64) || defined(__X86_64) || defined(__X86_64__)
	return "x86-64";
#elif defined(__LP64__) || defined(__LP64)
	return "x64";
#elif defined(__AMD32__) || defined(__AMD32)
	return "amd32";
#elif defined(__ia32__) || defined(__ia32)
	return "ia32";
#elif defined(i386) || defined(__i386) || defined(__i386__)
	return "i386";
#elif defined(i486) || defined(__i486) || defined(__i486__)
	return "i486";
#elif defined(i586) || defined(__i586) || defined(__i586__)
	return "i586";
#elif defined(i686) || defined(__i686) || defined(__i686__)
	return "i686";
#elif defined(_M_IX86)
	return "x86";
#elif defined(sparc) || defined(__sparc) || defined(__sparc__)
	return "sparc";
#elif defined(mc68000) || defined(__mc68000) || defined(__mc68000__)
	return "mc68000";
#elif defined(_IBMR2) || defined(___IBMR2) || defined(___IBMR2__)
	return "ibm";
#elif defined(_POWER) || defined(___POWER) || defined(___POWER__) || defined(_M_PPC)
	return "powerpc";
#elif defined(alpha) || defined(__alpha) || defined(__alpha__)
	return "alpha";
#else
    return "unknown-platform";
#endif
}

inline const std::string Version::toString() const
{
     std::string v;

     v = "3";
     v += "." + Convert::toString(this->majorV);
     v += "." + Convert::toString(this->minorV);
     v += "_" + Convert::toString(this->revision);

     // TODO: this series of "if" statements is useless, because majorV, minorV and revision are all unsigned integers
     /*
     if(this->majorV >= 0) 
     {
	  v += "." + Convert::toString(this->majorV);
	  if(this->minorV >= 0) 
	  {
	       v += "." + Convert::toString(this->minorV);
	       if(this->revision >= 0) 
	       {
		    v += "_" + Convert::toString(this->revision);
	       }
	  }
     }
     */

     if(this->stage != "release")
	  v += "-" + this->stage; 
     
     v += " \"" + releaseName + "\" [";
     v += this->getHardwarePlatform();
     v += "/";
     v += this->getOperatingSystem();
     v += "]";
     
     return v;
}

}

#endif

