﻿
#include <boost/asio.hpp>
#include <QTimer>

#include "syncobj.hpp"
#include "tianya_download.hpp"

tianya_download::tianya_download(boost::asio::io_service& io, const list_info& info, QObject* parent)
	: QObject(parent)
	, m_io_service(io)
	, m_tianya_context(std::make_shared<tianya_context>(std::ref(io)))
	, m_list_info(info)
	, m_first_chunk(true)
{
	m_connection_notify_chunk = m_tianya_context->connect_one_content_fetched([this](std::wstring content)
	{
		post_on_gui_thread([this, content]()
		{
			chunk_download_notify(QString::fromStdWString(content));

			if (m_first_chunk)
			{
				m_first_chunk = false;

				QTimer::singleShot(200, this, [this](){timed_first_timershot();});
			}
		});
	});

	m_connection_notify_complete = m_tianya_context->connect_download_complete([this]()
	{
		post_on_gui_thread([this]()
		{
			download_complete();
		});
	});
}

tianya_download::~tianya_download()
{
	m_tianya_context->stop();
}

void tianya_download::start()
{
	m_tianya_context->start(m_list_info.post_url);
}
