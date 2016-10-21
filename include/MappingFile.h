/******************************************************************************
 * $Id: FastFile.h, v 1.0 date: 04/26/2007 江万寿 Exp$
 *
 * Project:  
 * Purpose:  declarations of FastFile
 * Author:   江万寿, wsjws@163.com
 *
 ******************************************************************************
 * Copyright (c) 2007, 江万寿
 *
 *		未得到授权请勿拷贝
 *
 ******************************************************************************
 *
 * $Log: FastFile.h,v $
 *
 * Revision 1.1  04/26/2007  江万寿
 * 改进程序使可以映射大文件的一部分，进行动态映射
 *
 * Revision 1.2  10/24/2008  张靖	
 *	支持写文件。写文件时注意，写完后要调用FinishWriting()，将文件大小调整为实际大小
 *
 * Revision 1.3  05/11/2009  张靖
 * 为实现文件共享，增加函数接口。对同一个文件建立多个文件映射，共享文件
 *
 * Revision 1.4  06/08/2009	张靖
 * 映射长度不能超过文件长度，否则无法映射。
 */
#if !defined(AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_)
#define AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_

// #if _MSC_VER >= 1000
// #pragma once
// #endif // _MSC_VER >= 1000

static void ErrorMsg()
{
	LPVOID lpMsgBuf;
	char msg[80];
	
	FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(), 
		LANG_NEUTRAL, (LPTSTR)&lpMsgBuf,
		0,
		NULL );	
	
	sprintf( msg, "Error: %s", lpMsgBuf );
	LocalFree( lpMsgBuf );
	
	AfxMessageBox( msg, MB_OK);
}

const	DWORD	Default_BytesToMap=256*1024*1024;			// 64k的倍数	内存映射的块大小：256Mb

#define  ModeRead	1
#define  ModeWrite	2
#define	 ModeUpdate	3

class CMappingFile  
{
private:

	CString	m_fileName;
	HANDLE	m_inHFile;
	HANDLE  m_hFile;
	HANDLE  m_hFileMapping;

	DWORD	m_accessMode;

	LPBYTE	m_pMappedAdress;	// 存取指针

	__int64 m_accessOffset;		// 文件存取偏移量

	__int64	m_fileSize;			// 文件大小
	__int64	m_mapOffset;		// 映射起点

	long 	m_mapSize;			// 当前内存映射大小
	long	m_BytesToMap;		// 设定的内存映射大小
	int		m_mode;

	bool	m_bMapPartDirty;	//映射块是否被更新

private:
	void MapPartOfFile()
	{
		if( m_pMappedAdress )
			UnmapViewOfFile( m_pMappedAdress );
		
		m_pMappedAdress = (LPBYTE)MapViewOfFile( m_hFileMapping, m_accessMode, 
			DWORD(m_mapOffset>>32), DWORD(m_mapOffset & 0xffffffff), m_mapSize );
		
		//////////////////////////////////////////////////////////////////////////
		if( m_pMappedAdress == NULL ) 
		{
			DWORD err=GetLastError();
			ErrorMsg();

			MEMORYSTATUS	mem;
			GlobalMemoryStatus(&mem);
		}

		if(m_mode==ModeUpdate)
		{
			m_bMapPartDirty=false;
		}
	};
	void AdjustMapAddress( long bytes)
	{
		if( m_mapSize == 0 )	// map whole file	映射整个文件，不用调整映射地址
			return ;
		
		if( bytes > m_BytesToMap )		//读取长度大于映射块长度
		{
			char msg[80];
			sprintf( msg, "Error : Access size > mapped memory size ( %ld )", m_BytesToMap );
			AfxMessageBox( msg, MB_OK);
			exit(1);
		}
		
		if( NULL == m_pMappedAdress || m_accessOffset < m_mapOffset		//读写位置不在映射块中
			|| m_accessOffset+bytes > m_mapOffset + m_mapSize			//bytes为正，向后跳一块
			|| m_accessOffset+bytes < m_mapOffset)						//bytes为负值，向前跳一块
		{
			if(m_mode==ModeWrite)
			{
				//将文件映射强制更新到磁盘
				FlushViewOfFile(m_pMappedAdress, m_mapSize);
			}
			if(m_mode==ModeUpdate && m_bMapPartDirty)
			{
				FlushViewOfFile(m_pMappedAdress, m_mapSize);
			}
			
			// 调整使新的映射起点使当前读取块位于映射中心
			m_mapOffset = m_accessOffset + (bytes - m_BytesToMap)/2;
			
			if( m_mapOffset < 0L )
				m_mapOffset = 0L;

			//位移值必须是系统的分配粒度的倍数(迄今为止,Windows的所有实现代码的分配粒度均为64KB)
			m_mapOffset = m_mapOffset/ (64*1024)*(64*1024);	

			if( m_fileSize > 0  && m_mapOffset + m_BytesToMap > m_fileSize )
			{
				m_mapSize = long((m_fileSize - m_mapOffset) & 0xffffffff);
			}
			else
			{
				m_mapSize = m_BytesToMap;
			}
			
			
			MapPartOfFile();
		}
	}; 
	//写文件时，扩展文件大小，并重新映射内存
	void AppendMapView( long bytes)
	{
		if( bytes > m_mapSize )		//写入长度大于映射块长度
		{
			char msg[80];
			sprintf( msg, "Error : Access size > mapped memory size ( %ld )", m_mapSize );
			AfxMessageBox( msg, MB_OK);
			exit(1);
		}
		
		if( NULL == m_pMappedAdress || m_accessOffset < m_mapOffset 
			|| m_accessOffset+bytes > m_mapOffset + m_mapSize ) 
		{
			//将文件映射强制更新到磁盘
			FlushViewOfFile(m_pMappedAdress, m_mapSize);
			// 调整使新的映射起点使当前读取块位于映射中心
			m_mapOffset = m_accessOffset + (bytes - m_mapSize)/2;
			
			if( m_mapOffset < 0L )
				m_mapOffset = 0L;
			
			//位移值必须是系统的分配粒度的倍数(迄今为止,Windows的所有实现代码的分配粒度均为64KB)
			m_mapOffset = m_mapOffset/ (64*1024)*(64*1024);	

		//	DWORD dwFileSizeLo = 0; 
		//	DWORD dwSizeHigh = 0; 
		//	dwFileSizeLo = GetFileSize(m_hFile,&dwSizeHigh); 

		//	DWORD	FilePointer=SetFilePointer (m_hFile, 0, 0, FILE_CURRENT);
			long	DistanceToMoveHigh = long((m_mapOffset+m_mapSize)>>32);
			SetFilePointer (m_hFile, long((m_mapOffset+m_mapSize)&0xffffffff), &DistanceToMoveHigh, FILE_BEGIN);
			SetEndOfFile(m_hFile);
			DWORD dwSizeHigh = 0;
			m_fileSize = GetFileSize(m_hFile,&dwSizeHigh); 
			m_fileSize += dwSizeHigh << 32;

			if(m_mapOffset+m_mapSize>m_fileSize)
			{
				char msg[80];
				sprintf( msg, "Error : map size > file size ( %ld )", m_mapSize );
				AfxMessageBox( msg, MB_OK);
				exit(1);
			}

			Close();

			m_hFile = CreateFile( m_fileName.GetBuffer(0), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS /*| FILE_FLAG_WRITE_THROUGH*/, 0 );
			m_inHFile = INVALID_HANDLE_VALUE; 
//			err=GetLastError();

			if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
			{
				char msg[256];
				sprintf(msg, "Creating file %s failed!\r\n",m_fileName.GetBuffer(0) );
				AfxMessageBox(msg,MB_OK);
				return;
			}
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL );

			MapPartOfFile();
		}
	}; 

public:

	CMappingFile()
	{
		m_inHFile = INVALID_HANDLE_VALUE;
		m_hFile = INVALID_HANDLE_VALUE;
		
		m_hFileMapping = NULL;
		
		m_pMappedAdress = NULL;
		
		m_accessMode = FILE_MAP_READ;
		m_accessOffset = 0L;
		
		m_mapOffset = 0L;				// 映射起点
		m_BytesToMap = Default_BytesToMap;
		m_mapSize	= m_BytesToMap;
		m_fileSize  = 0L;
	};

	virtual ~CMappingFile()
	{
		Close();
	};

	//2009.10.9设置文件映射大小
	void SetMappingSize(DWORD dwMapSize)
	{
		m_BytesToMap=dwMapSize;
	};
	
	
	bool Create( const char *fileName )
	{
		char msg[256];
		
		Close();

		m_accessMode = FILE_MAP_READ;
		m_accessOffset = 0L;
		
		m_mapOffset = 0L;				// 映射起点
//		m_BytesToMap = Default_BytesToMap;
//		m_mapSize	= m_BytesToMap;
		m_fileSize  = 0L;
		
		// 打开文件
		m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
			NULL, CREATE_ALWAYS, FILE_FLAG_RANDOM_ACCESS, 0 );

		m_fileName=fileName;
		
		m_inHFile = INVALID_HANDLE_VALUE; 
		
		if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
		{
			sprintf(msg, "Creating file %s failed!\r\n",fileName );
			AfxMessageBox(msg,MB_OK);
			return FALSE;
		}

		//m_fileSize = 0x4000000;		//64M
		m_mapOffset = 0L;
		m_accessMode = FILE_MAP_WRITE;
		m_accessMode = FILE_MAP_ALL_ACCESS;
		m_accessOffset=0;
		m_fileSize = m_mapSize = m_BytesToMap;	//为新创建的文件设定初始大小
		m_mode=ModeWrite;
		
		// 建立影像内存映像handle
		m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, m_mapSize, NULL );
		
		MapPartOfFile();
		
		return TRUE;
	};
	bool Open( const char *fileName, char *mode/*'r','w','u'*/, char *lpMapName = NULL, HANDLE hFile = INVALID_HANDLE_VALUE)
	{
		char msg[256];
		
		Close();

		m_accessMode = FILE_MAP_READ;
		m_accessOffset = 0L;
		
		m_mapOffset = 0L;				// 映射起点
//		m_BytesToMap = Default_BytesToMap;
//		m_mapSize	= m_BytesToMap;
		m_fileSize  = 0L;

		//文件长度判断，映射整个文件还是分块映射
		OFSTRUCT fdOFSTRUCT;
		HFILE hfd = OpenFile(fileName, &fdOFSTRUCT, OF_READ);
		if (hfd == HFILE_ERROR) 
		{
			TRACE("Couldn't open %s!\n", fileName);
			return false;
		}

		BY_HANDLE_FILE_INFORMATION fdInfo;
		if (!GetFileInformationByHandle((HANDLE)hfd, &fdInfo))
			return false;
	
		DWORD len = fdInfo.nFileSizeLow;
		if (len <= m_BytesToMap) 
		{
			m_mapSize = len;
		} 
		else
		{
			m_mapSize = m_BytesToMap;
		}
		_lclose(hfd);
		
		// 打开文件
		if( hFile == INVALID_HANDLE_VALUE ) 
		{
			if( strchr( mode, 'w' ) ) 
			{//读写打开
				m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeWrite;
			}
			else if(strchr( mode, 'u' ))
			{
				m_hFile = CreateFile( fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
					OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeUpdate;
			}
			else 
			{
				m_hFile = CreateFile( fileName, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING , FILE_FLAG_RANDOM_ACCESS, 0 );
				
				m_mode=ModeRead;
			}
			
			
			if( m_hFile == INVALID_HANDLE_VALUE || m_hFile == (HANDLE)HFILE_ERROR )
			{
				DWORD err=GetLastError();
				sprintf(msg, "Opening file failed! %s\r\n",fileName );
				AfxMessageBox(msg,MB_OK);
				return FALSE;
			}
			m_inHFile = INVALID_HANDLE_VALUE; 
		}
		else
		{
			m_inHFile = m_hFile = hFile;
		}
		
		// 建立影像内存映像handle
		if( strchr( mode, 'w' ) || strchr( mode, 'u' )) 
		{	
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, 0, lpMapName );
			if( m_hFileMapping == NULL )
			{
				sprintf(msg, "CreateFileMapping for writing failed!\r\n",fileName );
				AfxMessageBox(msg,MB_OK);
				return FALSE;
			}
			DWORD dwSizeHigh;
			m_fileSize = GetFileSize( m_hFile, &dwSizeHigh );
			m_fileSize += dwSizeHigh << 32;
			m_accessMode = FILE_MAP_ALL_ACCESS;
		}
		else
		{
			m_hFileMapping = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, lpMapName );
			if( m_hFileMapping == NULL )
			{
				sprintf(msg, "CreateFileMapping for reading failed!\r\n",fileName );
				AfxMessageBox(msg,MB_OK);
				return FALSE;
			}
			
			DWORD dwSizeHigh;
			
			m_fileSize = GetFileSize( m_hFile, &dwSizeHigh );
			
			m_fileSize += dwSizeHigh << 32;
			
			if( m_fileSize < m_mapSize )
				m_mapSize = 0;
			
			m_accessMode = FILE_MAP_READ;
		}
		
		m_mapOffset = 0L;
		
		MapPartOfFile();
		
		return TRUE;
	};
	void Close()
	{
		if( m_pMappedAdress )
			UnmapViewOfFile( m_pMappedAdress );
		
		if( m_hFileMapping  ) 
			CloseHandle( m_hFileMapping );
		
		if( m_inHFile == INVALID_HANDLE_VALUE )
			if( m_hFile  ) 
				CloseHandle( m_hFile );
			
			m_pMappedAdress = NULL;
			m_hFileMapping = NULL;
			m_hFile = INVALID_HANDLE_VALUE;
	};

	bool Read( void *buf, long bytes)
	{
		AdjustMapAddress( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		// 不够64k的末尾部分？
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 修改  zj
			// 对于原映射方法，数据末尾不满64k的记录，每次都是取的最后一条记录
			// 修改为最后一块的内存映射用实际映射大小
			DWORD dwCurrentFilePosition=SetFilePointer( 
							m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
							0,     // do not move pointer 
							NULL,  // hFile is not large enough to need this pointer 
							FILE_CURRENT);  // provides offset from current position 

			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
			DWORD numberOfBytesRead;
			if( ReadFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
				ErrorMsg();

			ASSERT(false);
		}
		else	
		{//m_accessOffset在整个文件中的偏移值，m_mapOffset当前块起始处偏移值
			memcpy(buf, m_pMappedAdress + (m_accessOffset-m_mapOffset), bytes);
		}
		m_accessOffset += bytes;
		
		return true;
	}; 

	//调用方式与Read相同，不进行内存拷贝，直接返回指针
	void* ReadPtr(long bytes)
	{
		AdjustMapAddress( bytes );
		void *buf=0;
		
		if( m_pMappedAdress == NULL )
			return 0;
		
		// 不够64k的末尾部分？
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 修改  zj
			// 对于原映射方法，数据末尾不满64k的记录，每次都是取的最后一条记录
			// 修改为最后一块的内存映射用实际映射大小
			DWORD dwCurrentFilePosition=SetFilePointer( 
				m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
				0,     // do not move pointer 
				NULL,  // hFile is not large enough to need this pointer 
				FILE_CURRENT);  // provides offset from current position 
			
			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
//			DWORD numberOfBytesRead;
//			if( ReadFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
//				ErrorMsg();
			ASSERT(false);
		}
		else	
		{//m_accessOffset在整个文件中的偏移值，m_mapOffset当前块起始处偏移值
		
			buf=m_pMappedAdress + (m_accessOffset-m_mapOffset);
		}
		m_accessOffset += bytes;
		
		ASSERT(buf);
		
		return buf;
	};

	bool Write(const void *buf, long bytes)
	{//以create方式打开文件，并向文件中增加新的内容
		AppendMapView( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		memcpy(m_pMappedAdress+ (m_accessOffset-m_mapOffset), buf, bytes);
		m_accessOffset += bytes;
		
		return true;
	}; 

	bool Update(void *buf, long bytes)
	{//以rw方式打开，更新指定字段
		AdjustMapAddress( bytes );
		
		if( m_pMappedAdress == NULL )
			return FALSE;
		
		// 不够64k的末尾部分？
		if( m_mapSize > 0 &&  m_accessOffset + bytes > m_mapOffset + m_mapSize )	
		{//bug	
			//2008.10.22 修改  zj
			// 对于原映射方法，数据末尾不满64k的记录，每次都是取的最后一条记录
			// 修改为最后一块的内存映射用实际映射大小
			DWORD dwCurrentFilePosition=SetFilePointer( 
				m_hFile, // must have GENERIC_READ and/or GENERIC_WRITE 
				0,     // do not move pointer 
				NULL,  // hFile is not large enough to need this pointer 
				FILE_CURRENT);  // provides offset from current position 
			
			dwCurrentFilePosition=SetFilePointer( m_hFile, -bytes, NULL, FILE_END );
			
			DWORD numberOfBytesRead;
			if( WriteFile( m_hFile, buf, bytes, &numberOfBytesRead, NULL ) == FALSE )
				ErrorMsg();
		}
		else	
		{//m_accessOffset在整个文件中的偏移值，m_mapOffset当前块起始处偏移值
			memcpy(m_pMappedAdress + (m_accessOffset-m_mapOffset), buf, bytes);
		}
		m_accessOffset += bytes;
		m_bMapPartDirty=true;
		return true;
	};
	
	

	//写文件结束后调用，调整文件大小为实际大小
	void FinishWriting()
	{
		FlushViewOfFile(m_pMappedAdress, m_mapSize);

		if( m_pMappedAdress )	
		{
			UnmapViewOfFile( m_pMappedAdress );
			m_pMappedAdress = NULL;
		}
		
		if( m_hFileMapping  ) 
		{
			CloseHandle( m_hFileMapping );
			m_hFileMapping = NULL;
		}
		
		long	DistanceToMoveHigh = long(m_accessOffset>>32);
		SetFilePointer (m_hFile, long(m_accessOffset & 0xffffffff), &DistanceToMoveHigh, FILE_BEGIN); 
		
		SetEndOfFile(m_hFile);
		
		if( m_hFile  )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}
	};

	void ReMapping()
	{//重新映射当前块, 如果数据被更新后，重新读取文件中的数据
		MapPartOfFile();
	};

	void FlushViewPage()
	{//将当前页面内容强制更新到文件中
		BOOL	flag;
		flag=FlushViewOfFile(m_pMappedAdress, m_mapSize);

		if(flag==0)
		{
			DWORD err=GetLastError();
		}
	};

	void Seek(__int64 offset, int origin )
	{
		switch( origin )
		{
		case SEEK_CUR:
			m_accessOffset += offset;
			break;
		case SEEK_SET:
			m_accessOffset = offset;	
			break;
		case SEEK_END:
			ASSERT(false);
		}
	};

	//从当前位置跳过offset个字节
	//offset可正可负
	void Skip(long offset)
	{
		AdjustMapAddress( offset );
		m_accessOffset += offset;
	};

	void Remove()
	{
		if( m_pMappedAdress )	
		{
			UnmapViewOfFile( m_pMappedAdress );
			m_pMappedAdress = NULL;
		}
		
		if( m_hFileMapping  ) 
		{
			CloseHandle( m_hFileMapping );
			m_hFileMapping = NULL;
		}
		
		SetFilePointer (m_hFile, 0L, NULL, FILE_BEGIN); 
		
		SetEndOfFile(m_hFile);
		
		if( m_hFile  )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}
	};

	LPBYTE GetMapAddress()
	{//映射块的起始地址
		return	m_pMappedAdress;
	}
	
	//取文件映射对象
	HANDLE  GetFileMapping()
	{
		return	m_hFileMapping;
	}
};

#endif // !defined(AFX_FASTFILE_H__583948C4_4CC3_11D4_BBA6_0050BAAF35F4__INCLUDED_)

