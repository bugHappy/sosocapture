#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
};
//ffmpegÊ¹ÓÃutf8±àÂëµÄ
class CFFmpegString
{
public:
	CFFmpegString() {};
	CFFmpegString(LPCSTR pszSrc)
	{
		if (pszSrc)
		{
			wchar_t *pszTemp = ANSIToUnicode(pszSrc);
			strUTF8 = UnicodeToUtf8(pszTemp);
			delete pszTemp;
		}
	}
	CFFmpegString(LPCWSTR pszSrc) :strUTF8(nullptr)
	{
		if (pszSrc)
		{
			strUTF8 = UnicodeToUtf8(pszSrc);
		}
	}
	~CFFmpegString()
	{
		if (strUTF8)
		{
			delete strUTF8;
			strUTF8 = nullptr;
		}
	}
	CFFmpegString& operator =(LPCSTR pszSrc)
	{
		if (strUTF8)
		{
			delete strUTF8;
			strUTF8 = nullptr;
		}
		if (pszSrc)
		{
			wchar_t *pszTemp = ANSIToUnicode(pszSrc);
			strUTF8 = UnicodeToUtf8(pszTemp);
			delete pszTemp;
		}
		return (*this);
	}
	CFFmpegString& operator =(LPCWSTR pszSrc)
	{
		if (strUTF8)
		{
			delete strUTF8;
			strUTF8 = nullptr;
		}
		if (pszSrc)
		{
			strUTF8 = UnicodeToUtf8(pszSrc);
		}
		return (*this);
	}
	char *operator()(void)
	{
		return strUTF8;
	}
	operator LPSTR()
	{
		return strUTF8;
	}
	void CopyTo(char **pszStr)
	{
		if (strUTF8)
		{
			size_t size = strlen(strUTF8) + 1;
			*pszStr = new char[size];
			memcpy_s(*pszStr, size, strUTF8, size);
		}
		else
			*pszStr = nullptr;
	}
protected:
	wchar_t* ANSIToUnicode(const char* pszSrc)
	{
		int nUnicodeSize = MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, nullptr, 0);
		wchar_t *pszUnicode = new wchar_t[nUnicodeSize + 1];
		memset(pszUnicode, 0, (nUnicodeSize + 1) * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, pszSrc, -1, pszUnicode, nUnicodeSize);
		return pszUnicode;
	}
	char *UnicodeToUtf8(const wchar_t* pszSrc)
	{
		int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, pszSrc, -1, nullptr, 0, nullptr, nullptr);
		char *pszUtf8 = new char[nUTF8codeSize + 1];
		memset(pszUtf8, 0, nUTF8codeSize + 1);
		WideCharToMultiByte(CP_UTF8, 0, pszSrc, -1, pszUtf8, nUTF8codeSize, nullptr, nullptr);
		return pszUtf8;
	}
private:
	char *strUTF8 = nullptr;
};

class CAVFormatContext
{
public:
	CAVFormatContext()
	{
		m_pFormatCtx = avformat_alloc_context();
	}
	~CAVFormatContext()
	{
		if (m_pFormatCtx)
		{
			avformat_free_context(m_pFormatCtx);
		}
	}
	operator AVFormatContext*()
	{
		return m_pFormatCtx;
	}
	int OpenInput(LPCTSTR url, AVInputFormat *fmt = nullptr, AVDictionary **options = nullptr)
	{
		CFFmpegString FsUrl = url;
		return avformat_open_input(&m_pFormatCtx, FsUrl, fmt, options);
	}
	int OpenInput(LPCTSTR url, LPCTSTR pcsfmt = nullptr, AVDictionary **options = nullptr)
	{
		CFFmpegString FsFmt = pcsfmt;
		AVInputFormat *fmt = av_find_input_format(FsFmt);
		CFFmpegString FsUrl = url;
		return avformat_open_input(&m_pFormatCtx, FsUrl, fmt, options);
	}
	int OpenOutput(TCHAR *url, AVOutputFormat *ofmt = nullptr, const char *formatname = nullptr)
	{
		CFFmpegString FsUrl = url;
		return avformat_alloc_output_context2(&m_pFormatCtx, ofmt, formatname, FsUrl);
	}
private:
	AVFormatContext	*m_pFormatCtx = nullptr;
};
class CAVCodec
{
public:
	AVCodec *FindEncoder(LPCTSTR codec_name)
	{
		CFFmpegString FsCodecName = codec_name;
		m_Codec = avcodec_find_encoder_by_name(FsCodecName);
		return m_Codec;
	}
	AVCodec *FindEncoder(enum AVCodecID id)
	{
		m_Codec = avcodec_find_encoder(id);
		return m_Codec;
	}
	operator AVCodec*()
	{
		return m_Codec;
	}
private:
	AVCodec *m_Codec=nullptr;
};
class CAVCodecContext
{
public:
	AVCodecContext *Alloc_Context(AVCodec *codec)
	{
		AVCodecContext* m_CodecContext = avcodec_alloc_context3(codec);
		return m_CodecContext;
	}

	operator AVCodecContext*()
	{
		return m_CodecContext;
	}
	~CAVCodecContext()
	{
		if (m_CodecContext)
			avcodec_free_context(&m_CodecContext);
	}
private:
	AVCodecContext *m_CodecContext=nullptr;
};

class CAvFrame
{
public:
	AVFrame* Alloc()
	{
		m_Frame = av_frame_alloc();
		return m_Frame;
	}
	int GetBuf(int align)
	{
		return av_frame_get_buffer(m_Frame, align);
	}
	operator AVFrame*()
	{
		return m_Frame;
	}
	void UnRef()
	{
		if(m_Frame)
			av_frame_unref(m_Frame);
	}
	~CAvFrame()
	{
		if (m_Frame)
			av_frame_free(&m_Frame);
	}
private:
	AVFrame *m_Frame=nullptr;
};
class CAvPacket
{
public:
	AVPacket* Alloc()
	{
		m_Pkt = av_packet_alloc();
		return m_Pkt;
	}
	operator AVPacket*()
	{
		return m_Pkt;
	}
	~CAvPacket()
	{
		if(m_Pkt)
			av_packet_free(&m_Pkt);
	}
private:
	AVPacket *m_Pkt=nullptr;
};

class CFFmpegWarpper
{
	int encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
		FILE *outfile)
	{
		int ret;

		/* send the frame to the encoder */
		if (frame)
			return -1;

		ret = avcodec_send_frame(enc_ctx, frame);
		if (ret < 0) {
			return ret;
		}

		while (ret >= 0) {
			ret = avcodec_receive_packet(enc_ctx, pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
				return 0;
			else if (ret < 0) {
				return ret;
			}
			fwrite(pkt->data, 1, pkt->size, outfile);
			av_packet_unref(pkt);
		}
		return ret;
	}
public:
	CFFmpegWarpper();
	~CFFmpegWarpper();
};

