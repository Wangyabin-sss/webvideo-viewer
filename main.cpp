#include <iostream>
#include <fstream>
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <json.hpp>

#ifdef __cplusplus
extern "C"
{
    #include <stdio.h>
    #include <dirent.h>
    #include <errno.h>
}
#endif

using namespace std;
using njson = nlohmann::json;

// <center>
//    <img src="123.jpg" width="800px">
//    <video controls width="800"><source src="video3.mp4" type="video/mp4"></video>
// </center>
	
string index_htmlstart = R"(
<! DOCTYPE HTML>
<html>
<head>
<title>video browser</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<style>
      .video-container {
        display: flex;
        flex-wrap: wrap;
        justify-content: space-between;
      }
      .video-item {
        width: 30%;
        margin-bottom: 20px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);
        border-radius: 10px;
        overflow: hidden;
      }
      .video-thumbnail {
        width: 100%;
        height: 0;
        padding-bottom: 75%;
        background-position: center;
        background-repeat: no-repeat;
        background-size: cover;
        cursor: pointer;
        box-shadow: 0 0 20px #dda;
      }
      .video-description {
        padding: 10px;
        background-color: #f2f2f2;
      }
    </style>
</head>    
<body>)";
string index_htmlend = R"(</body><script>
    function openVideo(videoUrl) {
        window.open(videoUrl, '_self');
    }
    function openDir(videoUrl) {
      window.open(videoUrl, '_self');
    }
    function openOther(videoUrl) {
      window.open(videoUrl, '_blank');
    }
  </script></html>)";
string htmltmp;

string basedir="./www";

int check_file_type(char *filename)
{
    if(strstr(filename, ".avi") != NULL)
        return 1;
    else if(strstr(filename, ".mp4") != NULL)
        return 2;
    else if(strstr(filename, ".ogg") != NULL)
        return 3;
    else 
        return 0;
}

//生成html中body里的内容
string getdir_detail(string dirpath, int listnum, int width)
{
    string html="";
    DIR *dir;
    struct dirent *ptr;
    int numtmp=0;    //用于显示几列数据
    printf("try to open-%s\n",dirpath.c_str());
    if ((dir=opendir(dirpath.c_str())) == NULL)
    {    
        perror("Open dir error...");
        return html;
    }
    while ((ptr=readdir(dir)) != NULL)
    {
        if(numtmp == 0)
        {
            html.append("<div class=\"video-container\">\r\n");
        }
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    //current dir OR parrent dir
        {
            if(strcmp(ptr->d_name,"..")==0)
            {
                string tmp = dirpath;
                if(tmp!="./www/video/")
                {
                    tmp.pop_back();
                    while(tmp.substr(tmp.length()-1)!="/")
                    {
                        tmp.pop_back();
                    }
                    html.append("<div class=\"video-item\">");
                    html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/dir.jpg);\" onclick=\"openDir('"+tmp.substr(strlen("./www"))+"')\"></div>");
                    html.append("<div class=\"video-description\">");
                    html.append("<h3>Video 1 Title</h3>");
                    html.append("<p>上一级目录"+tmp.substr(strlen("./www"))+"</p>\r\n</div>\r\n</div>");
                }
                else
                {
                    html.append("<div class=\"video-item\">");
                    html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/dir.jpg);\"></div>");
                    html.append("<div class=\"video-description\">");
                    html.append("<h3>Video 1 Title</h3>");
                    html.append("<p>/</p>\r\n</div>\r\n</div>");
                }
            }
            else
            {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/ff.jpg);\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>Video 1 Title</h3>");
                html.append("<p>刷新</p>\r\n</div>\r\n</div>");
            }
        }
        else if(ptr->d_type == 8)    //file
        {
            int ret=0;
            if(ret = check_file_type(ptr->d_name)) {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/video.jpg);\" onclick=\"openVideo('"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"')\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"</h3>");
                html.append("<p>video file</p>\r\n</div>\r\n</div>");
            }
            else {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/unknown.jpg);\" onclick=\"openOther('"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"')\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"</h3>");
                html.append("<p>not a video file</p>\r\n</div>\r\n</div>");
            }
        }
        else if(ptr->d_type == 4)    //dir
        {
            html.append("<div class=\"video-item\">");
            html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/dir.jpg);\" onclick=\"openDir('"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"/')\"></div>");
            html.append("<div class=\"video-description\">");
            html.append("<h3>Video 1 Title</h3>");
            html.append("<p>下一级目录"+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"/</p>\r\n</div>\r\n</div>");
        }
        numtmp++;
        if(numtmp==listnum) {
            html.append("</div>\r\n");
            numtmp = 0;
        }
    }
    if(numtmp!=0)
    {
        html.append("<div class=\"video-item\">");
        html.append("<form method=\"POST\" action=\"/makedir\" accept-charset=\"utf-8\">");
		html.append("<input type=\"text\" id=\"text-input\" name=\"text-input\" placeholder=\"在这里输入要提交的文本\">"); 
        html.append("<input type=\"hidden\" name=\"path-name\" value=\""+dirpath+"\">");
        html.append("<button type=\"submit\">提交</button> </form>");
        html.append("<form method=\"POST\" action=\"/fileupload"+dirpath.substr(1)+"\" enctype=\"multipart/form-data\">");
        html.append("<div><label for=\"file\">Choose file to upload</label>");
        html.append("<input type=\"file\" id=\"file\" name=\"file\" multiple /></div>");
        html.append("<div><button>Submit</button></div></form></div>");
        html.append("</div>\r\n");
    }
    else
    {
        html.append("<div class=\"video-container\">\r\n");
        html.append("<div class=\"video-item\">");
        html.append("<form method=\"POST\" action=\"/makedir\" accept-charset=\"utf-8\">");
		html.append("<input type=\"text\" id=\"text-input\" name=\"text-input\" placeholder=\"在这里输入要提交的文本\">");
        html.append("<input type=\"hidden\" name=\"path-name\" value=\""+dirpath+"\">"); 
        html.append("<button type=\"submit\">提交</button> </form>");
        html.append("<form method=\"POST\" action=\"/fileupload"+dirpath.substr(1)+"\" enctype=\"multipart/form-data\">");
        html.append("<div><label for=\"file\">Choose file to upload</label>");
        html.append("<input type=\"file\" id=\"file\" name=\"file\" multiple /></div>");
        html.append("<div><button>Submit</button></div></form></div>");
        html.append("</div>\r\n");
    }
    
    closedir(dir);
    //cout<<html<<endl;
    return html;
}

void video_callback(const httplib::Request& req, httplib::Response& res) {
    string targetdir(basedir+req.path);
    //在最后添加符号  /
    if(targetdir.substr(targetdir.length()-1)!="/")
        targetdir.append("/");
    string tmp = getdir_detail(targetdir, 3, 300);
    htmltmp = "";
    htmltmp.append(index_htmlstart+tmp+index_htmlend);
    res.set_content(htmltmp, "text/html");
}

void file_upload(const httplib::Request& req, httplib::Response& res) {
    auto file = req.get_file_value("file");
    string filenamepath("."+req.path.substr(strlen("/fileupload"))+file.filename);

    cout<< "file length: " << file.content.length() << endl
        << "file name: " << filenamepath << endl;

    ofstream ofs(filenamepath, std::ios::binary);
    ofs << file.content;

    res.set_redirect(req.path.substr(strlen("/fileupload/www")), 302);
    res.set_content("", "text/html");
}
void make_dir(const httplib::Request& req, httplib::Response& res) {
    string gettext = req.get_param_value("text-input");
    string path = req.get_param_value("path-name");

    string cmd("mkdir "+path+gettext);
    system(cmd.c_str());

    res.set_redirect(path.substr(strlen("./www")), 302);
    res.set_content("", "text/html");
}

void error_callback(const httplib::Request& req, httplib::Response& res) {
    const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
}

int main(int argc, char *argv[])
{
    httplib::Server ser;
    if(!ser.is_valid())
    {
        cout<<"error"<<endl;
        return -1;
    }

    ser.set_base_dir(basedir);
    //允许video/的任意web路径进入回调函数处理
    ser.Get("/video/(.*)", video_callback);
    ser.Post("/fileupload/(.*)", file_upload);
    ser.Post("/makedir", make_dir);
    ser.set_error_handler(error_callback);

    ser.listen("0.0.0.0", 8080);
    return 0;
}

