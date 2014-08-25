#include "FileDetection.h"
#include "XMLInfo.h"
#include "TaskQueue.h"
#include "Configure.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <glog/logging.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

FileDetection::FileDetection(std::string path)
    : m_scanPath(path)
    , m_allFileSet(new StringSet)
{
    // 判断要扫描的目录是否存在
    if(!boost::filesystem::exists(m_scanPath))
    {
        LOG(ERROR) << "scan path [" << m_scanPath << "] is not exist." ;
        throw std::string("scan path is not exist.");
    }
}


FileDetection::~FileDetection(void)
{
}

void FileDetection::run(void)
{
	LOG(INFO) << "File detection begin ." ;

    while(true)
    {
        boost::shared_ptr<StringSet>  newSet(new StringSet);

        if(boost::filesystem::exists(m_scanPath))
        {
            boost::filesystem::directory_iterator itBegin(m_scanPath);
            boost::filesystem::directory_iterator itEnd;

            for(;itBegin != itEnd; itBegin++)
            {
                try
                {
                    if (boost::filesystem::is_directory(*itBegin))
                    {
                        // 暂时不支持子目录扫描
                        //boost::filesystem::path p = itBegin->path();
                        //PrintAllFile(p);
                    }
                    else
                    {
                        //检测这个文件是不是已经扫描过
                        std::string fileName(itBegin->path().string());
                        if(this->m_allFileSet->find(fileName) == m_allFileSet->end())
                        {
                            // 新发现文件，先检查个文件是否是我们要找的xml文件
                            XMLInfo xmlInfo;
                            if(xmlInfo.load(itBegin->path().string()))
                            {
                                if(xmlInfo.AssetName.empty())
                                {
                                    LOG(INFO)  << "Find : " << itBegin->path().string() << ".  but its TS file is empty" ;
                                }
                                else
                                {
                                    LOG(INFO)  << "Find : " << itBegin->path().string() << ".  its TS file : " <<
                                        xmlInfo.AssetName ;
                                    TaskQueue::instance().producer(itBegin->path().string(), xmlInfo.AssetName);
                                }
                            }
                        }
                        newSet->insert(fileName);
                    }
                }
                catch (const std::exception & ex )
                {
                    LOG(ERROR)<< ex.what() ;
                    continue;
                }
            }

            // 更新列表为这一次扫描的列表
            this->m_allFileSet = newSet;
        }
        else
        {
            LOG(INFO) << "scan path [" << m_scanPath << "] is not exist." ;
        }

#ifdef WIN32
        // 每隔指定时间，再扫描一次目录
        Sleep(SCAN_INTERVAL * 1000);
#else
#endif
    }
}

void FileDetection::start(void)
{
    m_thread.reset(new boost::thread( boost::bind(&FileDetection::run,this)));
}
