#include <iostream>
#define CPPHTTPLIB_OPENSSL_SUPPORT
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
</head>    
<body>)";
string index_htmlend = R"(</body></html>)";
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
            html.append("<center>\r\n");
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
                    html.append("<a href=\""+tmp.substr(strlen("./www"))+"\"><img src=\"/dir.jpg\"");
                    html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
                }
                else
                {
                    html.append("<a><img src=\"/dir.jpg\"");
                    html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
                }
            }
            else
            {
                html.append("<a><img src=\"/ff.jpg\"");
                html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
            }
        }
        else if(ptr->d_type == 8)    //file
        {
            int ret=0;
            if(ret = check_file_type(ptr->d_name)) {
                html.append("<video controls width=\""+to_string(width)+"\"><source src=\""+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"\" type=\"");
                if(ret == 1 || ret == 2)
                    html.append("video/mp4\"></video>\r\n");
                else if(ret == 3)
                    html.append("video/ogg\"></video>\r\n");
            }
            else {
                html.append("<span>"+string(ptr->d_name)+"  文件格式不支持"+"</span>\r\n");
            }
        }
        else if(ptr->d_type == 4)    //dir
        {
            html.append("<a href=\""+dirpath.substr(strlen("./www"))+string(ptr->d_name)+"\"><img src=\"/dir.jpg\"");
            html.append(" width=\""+to_string(width)+"px\"></a>\r\n");
        }
        numtmp++;
        if(numtmp==listnum) {
            html.append("<center>\r\n");
            numtmp = 0;
        }
    }
    if(numtmp!=0)
        html.append("<center>\r\n");
    
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

