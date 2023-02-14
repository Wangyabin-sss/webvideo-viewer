#include <iostream>
#include <vector>
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <windows.h>
#include <json.hpp>

#ifdef __cplusplus
extern "C"
{
    #include <stdio.h>
    #include <stdlib.h>
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
      window.open(videoUrl, '_blank');
    }
    function openDir(videoUrl) {
      window.open(videoUrl, '_self');
    }
    function openOther(videoUrl) {
      window.open(videoUrl, '_self');
    }
  </script></html>)";
string htmltmp;

string basedir="./www";

int check_file_type(const char *filename)
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

void from_txt_getname(string listtxt, vector<tuple<string,string>> &tname)
{
    FILE *fd=NULL;
    fd = fopen(listtxt.c_str(), "r");
    if(fd == NULL) {
      perror("打开文件时发生错误");
      return;
    }
    char fbuff[512]={0};
    while (fgets(fbuff, 512, fd) != NULL)
    {
        if(fbuff[0]!=' '&&fbuff[0]!='\n')
        {
            //cout<<fbuff;
            strtok(fbuff," ");
            strtok(NULL," ");
            string type(strtok(NULL," "));
            if(type=="<DIR>")
            {
                string name(strtok(NULL," "));
                name.pop_back();
                tname.push_back(make_tuple(type, name));
            }
            else
            {
                string name(strtok(NULL," "));
                name.pop_back();
                tname.push_back(make_tuple("<FILE>",name));
            }
        }
    }
    fclose(fd);
}

//生成html中body里的内容
string getdir_detail(string dirpath, int listnum, int width)
{
    string html="";
    string cmd("dir "+dirpath+" > ./list.txt");
    replace(cmd.begin(), cmd.end(), '/', '\\');
    //cout<<cmd.c_str()<<endl;
    system(cmd.c_str());

    vector<tuple<string,string>> files;
    from_txt_getname("./list.txt", files);
    
    int numtmp=0;
    for(size_t i=0;i<files.size();i++)
    {
        string type = get<0>(files[i]);
        string name = get<1>(files[i]);
        //cout<<get<0>(files[i])<<"--"<<get<1>(files[i])<<endl;
        if(numtmp == 0)
        {
            html.append("<div class=\"video-container\">\r\n");
        }
        if(type == "<DIR>")
        {
            if(name == "..")
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
            else if(name == ".")
            {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/ff.jpg);\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>Video 1 Title</h3>");
                html.append("<p>刷新</p>\r\n</div>\r\n</div>");
            }
            else
            {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/dir.jpg);\" onclick=\"openDir('"+dirpath.substr(strlen("./www"))+name+"/')\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>Video 1 Title</h3>");
                html.append("<p>下一级目录"+dirpath.substr(strlen("./www"))+name+"/</p>\r\n</div>\r\n</div>");
            }
        }
        else if(type == "<FILE>")
        {
            int ret=0;
            if(ret = check_file_type(name.c_str())) {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/video/123.jpg);\" onclick=\"openVideo('"+dirpath.substr(strlen("./www"))+name+"')\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>"+dirpath.substr(strlen("./www"))+name+"</h3>");
                html.append("<p>video file</p>\r\n</div>\r\n</div>");
            }
            else {
                html.append("<div class=\"video-item\">");
                html.append("<div class=\"video-thumbnail\" style=\"background-image: url(/video/123.jpg);\" onclick=\"openOther('"+dirpath.substr(strlen("./www"))+name+"')\"></div>");
                html.append("<div class=\"video-description\">");
                html.append("<h3>"+dirpath.substr(strlen("./www"))+name+"</h3>");
                html.append("<p>not a video file</p>\r\n</div>\r\n</div>");
            }
        }
        numtmp++;
        if(numtmp==listnum) {
            html.append("</div>\r\n");
            numtmp = 0;
        }
    }
    if(numtmp!=0)
        html.append("</div>\r\n");
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
    ser.Get(R"(/video/(.*))", video_callback);
    ser.set_error_handler(error_callback);

    ser.listen("0.0.0.0", 8080);
    return 0;
}

