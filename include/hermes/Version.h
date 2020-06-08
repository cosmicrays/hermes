#include <string>

extern const char g_GIT_SHA1[];
extern const char g_GIT_REFSPEC[];
extern const char g_GIT_DESC[];

/**
 @fn declare_version
 @brief A helper function to track the steering card version

 Use at the beginning of the (python) code by putting the string of
 the current version, e.g., declare_version("v0.9-1-ga343b23").
 If there is a mismatch a warning message is shown.
 The current version number can be obtained through git:
	git describe --tags
 or through python:
  	print(hermes.__version__)   
 */
void declare_version(const std::string);
