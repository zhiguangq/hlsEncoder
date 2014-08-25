#include "EncoderWorker.h"
#include "TaskQueue.h"
#include "Configure.h"
#include "NewXMLFile.h"

#include <iostream>
#include <glog/logging.h>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>

#include <Poco/PipeStream.h>
#include <Poco/Process.h>
#include <Poco/StreamCopier.h>
#include <boost/scoped_ptr.hpp>

EncoderWorker::EncoderWorker(void)
{
}


EncoderWorker::~EncoderWorker(void)
{
}

void EncoderWorker::run(void)
{
    while(true)
    {
        if(TaskQueue::instance().empty())
        {
            // 如果没有任务，就休眠1秒
            Sleep(1000);
            continue;
        }

        std::string xmlFile;
        std::string tsFile;
        TaskQueue::instance().consumer(xmlFile, tsFile);

		LOG(INFO) << "Start to encode : " << tsFile ;

        std::vector<streamInfo> bitrateVector;
        std::vector<std::string> args;
        Poco::Pipe outPipe;
        boost::scoped_ptr<Poco::ProcessHandle> m_processHandler;

		// 建立生成目录
		std::string subPath = tsFile.substr(0,tsFile.size() - 3);
		boost::filesystem::create_directories(Configure::instance().m_outputTSPath.objectName + "\\" + subPath);

        // 配置几个不同的输出流
		for(int i = 0; i < 3; i++)
		{
			if(Configure::instance().m_outputStream[i].enable.compare("1") == 0)
			{
				streamInfo s(Configure::instance().m_outputStream[i].bitrate,Configure::instance().m_outputStream[i].resolutionRatio, \
					Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\" + Configure::instance().m_outputStream[i].bitrate + ".mp4");
				bitrateVector.push_back(s);
			}
		}
		//streamInfo a("1200k","1280x720", Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\1200k.mp4");
		//streamInfo b("800k","720x576", Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\800k.mp4");
        //streamInfo c("400k","480x320", Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\400k.mp4");
        //bitrateVector.push_back(a);
        //bitrateVector.push_back(b);
        //bitrateVector.push_back(c);

        // 配置 ffmpeg 的输入参数
#ifdef WIN32
		args.push_back("-i " + Configure::instance().m_inputTSPath.objectName + "\\" + tsFile);
#else
#endif
        args.push_back("-y");
        args.push_back("-loglevel");
        args.push_back("quiet");

        for(int i = 0; i < bitrateVector.size(); i++)
        {
            args.push_back("-b:v");
            args.push_back(bitrateVector[i].m_bitRate);             // 设置码率
            args.push_back("-s");
            args.push_back(bitrateVector[i].m_resolutionRatio);     // 设置分辨率
            args.push_back("-vcodec");
            args.push_back("libx264");
            args.push_back("-ar");
            args.push_back("44100");
            args.push_back("-ac");
            args.push_back("2");
            args.push_back("-b:a");
            args.push_back("128k");
            args.push_back("-acodec");
#ifdef WIN32
            args.push_back("libvo_aacenc");   
#else
            args.push_back("aac");
#endif
            args.push_back("-threads");
            args.push_back("0");
            args.push_back(bitrateVector[i].m_outputName);          // 输出文件
        }

        // 正常的操作命令是以stdout输出的，但是ffmpeg程序的输出以在stderr为输出的。
        m_processHandler.reset(new Poco::ProcessHandle(Poco::Process::launch("ffmpeg", args, 0, 0, 0)));
        Poco::Process::wait(*m_processHandler);

		// 生成hls文件
		for(int i = 0; i < bitrateVector.size(); i++)
		{
            LOG(INFO) << "Start to encode HLS : " << bitrateVector[i].m_bitRate ;

			// 创建目录
			boost::filesystem::create_directories(Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\" + bitrateVector[i].m_bitRate);

			std::vector<std::string> hls_args;
#ifdef WIN32
			hls_args.push_back("-i " + bitrateVector[i].m_outputName);
#else
#endif
			hls_args.push_back("-bsf");
			hls_args.push_back("h264_mp4toannexb");
			hls_args.push_back("-vcodec");
			hls_args.push_back("copy");
			hls_args.push_back("-acodec");
			hls_args.push_back("copy");
			hls_args.push_back("-hls_time");
			hls_args.push_back("10");
			hls_args.push_back("-hls_list_size");
			hls_args.push_back("10000");
            hls_args.push_back("-loglevel");
            hls_args.push_back("quiet");
			hls_args.push_back(Configure::instance().m_outputTSPath.objectName + "\\" + subPath + "\\" + bitrateVector[i].m_bitRate + "\\" + bitrateVector[i].m_bitRate + ".m3u8");

			m_processHandler.reset(new Poco::ProcessHandle(Poco::Process::launch("ffmpeg", hls_args, 0, 0, 0)));

			Poco::Process::wait(*m_processHandler);
		}

        // 把xml文件删除，并写到指定目录
        boost::filesystem::path p(xmlFile);
		
        //boost::filesystem::copy_file(xmlFile, Configure::instance().m_outputXMLPath.objectName + "\\" + \
        //    p.leaf().filename().string() , boost::filesystem::copy_option::overwrite_if_exists);

        XMLInfo xml;
        bool writeOk = false;
        // 如果中间远程cifs断开链接，就要不断重连接
        while(!writeOk)
        {
            if(!xml.load(xmlFile))
            {
                // 休眠15秒再试
                Sleep(15 * 1000);
                continue;
            }

            try
            {
                // 建立xml目录
		        boost::filesystem::create_directories(Configure::instance().m_outputXMLPath.objectName + "\\" + subPath);

                // 生成总局格式
                NewXMLFile newXMLFile;
                newXMLFile.archiveXML(Configure::instance().m_outputXMLPath.objectName + "\\" + subPath + "\\" + p.leaf().filename().string(), \
                    xml.AssetName.erase(xml.AssetName.size() - 3,3), xml.EventName, xml.StartTime, xml.EndTime, xml.ChannelName);

                // 生成.fin文件
                std::ofstream finFile(Configure::instance().m_outputXMLPath.objectName + "\\" + subPath + "\\" + subPath + ".fin");

                boost::filesystem::remove(xmlFile);
                writeOk = true;
            }
            catch(...)
            {
                LOG(INFO) << "Write xml error. ";
                writeOk = false;
            }

            if(!writeOk)
            {
                // 休眠15秒再试
                Sleep(15 * 1000);
            }            
        }

        LOG(INFO) << "End encode : " << tsFile ;
    }
}

void EncoderWorker::start(void)
{
    boost::thread thrd( boost::bind(&EncoderWorker::run,this));
    //thrd.join();
}
