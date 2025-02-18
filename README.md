# Web Server

## Description
This is a simple web server implemented in C that serves static HTML files. It listens for incoming HTTP requests and responds with the appropriate content based on the requested URL.

## Evaluation Criteria Implementation
The server has been developed to meet the following evaluation criteria:

1. **Consistent Page Serving (50%)**
   - The server consistently serves the default home page for root URL requests
   - Implements proper HTTP response formatting for all requests

2. **Different File Handling (20%)**
   - Correctly serves different file types (HTML, images, audio, video) based on request
   - Supports multiple MIME types for proper content-type headers

3. **Error Handling (10%)**
   - Implements proper HTTP error responses (404, 415, 500)
   - Handles file not found and unsupported media type errors

4. **Multiple File Type Handling (10%)**
   - Supports serving of various file types including:
     - HTML documents
     - Images (JPG, PNG)
     - Audio (MP3)
     - Video (MP4)

5. **Optimizations (10%)**
   - Implements basic file caching for frequently accessed files
   - Uses efficient memory management for file handling


## Installation
To compile the server, ensure you have GCC installed. Run the following command in the Command Prompt:

```bash
 gcc -o server server.c src/http_utils.c src/mime_types.c -lws2_32
```

## Usage
To run the server, execute the following command:

```bash
./server
```

The server will start listening on port 8080. You can access the web pages by navigating to the following URLs in your web browser:

- `http://localhost:8080/` (home.html)
- `http://localhost:8080/contents.html` (contents.html)
- `http://localhost:8080/features.html` (features.html)

## License
This project is licensed under the MIT License.

## Created by:
iamkokilahiran

## Copyright Notice
 All rights reserved 2025.
