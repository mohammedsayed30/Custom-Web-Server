#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream> 
#include <stdio.h> 



// Function to extract the path from the HTTP request buffer
std::string extractUrlFromBuffer(const std::string& buffer) {
    //get the request that in buffer
    std::istringstream request(buffer);
    //to hold the request method and url and protocol
    std::string method, url, protocol;
    // Parse the HTTP request line
    request >> method >> url >> protocol; 
   //for get methods
    if (method == "GET") {
        return url; // Return the URL part of the request
    } else {
        std::cout << "Unsupported HTTP method." << std::endl;
        return ""; // Return an empty string for unsupported methods
    }
}

// Function to transform the URL into a filesystem path
std::string extractAndTransformPath(const std::string& url) {
    // Find the position of "8000/"
    size_t pos = url.find("8000/");

    if (pos != std::string::npos) {
        // Extract the substring starting from the character after "8000/"
        std::string path = url.substr(pos + 5); // 5 is the length of "8000/"

        // Replace forward slashes with backslashes
        std::replace(path.begin(), path.end(), '/', '\\');

        // Prepend the drive letter (e.g., "C:")
        return "C:" + path;
    } else {
        std::cout << "The URL does not contain '8000/'." << std::endl;
        return ""; // Return an empty string if "8000/" is not found
    }
}
/**
 * read the file content of a file
 * @param  address of the file path
 * @return the content of the file
 */

std::string readFileContent(const std::string& filePath) {
    //openeing the file for that exist in this  path
    std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
    if (!fileStream) {
        //if empty
        return "";
    }
    //reading the file content into a string stream
    std::ostringstream contentStream;
    contentStream << fileStream.rdbuf();
    //Returning the File Content as a String
    return contentStream.str();
}


