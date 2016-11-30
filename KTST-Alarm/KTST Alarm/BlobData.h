#pragma once

template<class _data_type>
class BlobData
{
public:
	typedef BlobData<_data_type> _MyT;

	BlobData() {
		m_pdata = NULL; 
		m_count = 0;
		m_size = 0;
	}

	~BlobData() {
		if(m_pdata) {
			delete [] m_pdata; 
			
			m_pdata = NULL; 
			m_size = 0;
			m_count = 0;
		}
	}

	void Reset() {
		if(m_pdata) {
			delete [] m_pdata;
			m_pdata = NULL;
			m_count = 0;
			m_size = 0;
		}
	}

	_data_type* SetData(const _data_type *pData, size_t nCount)
	{
		if(nCount <= 0) {
			return NULL;
		}

		Reset();

		size_t size = nCount * sizeof(_data_type);
		m_pdata = new _data_type[nCount];
		
		if(!pData) {
			memset(m_pdata, 0, size);
		}
		else {
			memcpy(m_pdata, pData, size);
		}		
	
		m_count = nCount;
		m_size = size;

		return m_pdata;
	}

	_data_type* GetData() const 
	{
		return m_pdata;
	}
	
	size_t GetSize() const
	{
		return m_size;
	}

	size_t GetCount() const 
	{
		return m_count;
	}

	bool IsDataValid() 
	{
		return (m_pdata != NULL && m_size > 0 && m_count > 0);
	}

	_MyT& operator =(const _MyT& other) {	
		if(this == &other)
			return * this;

		Reset();

		size_t size = other.GetSize();
		if(size > 0) {
			size_t count = other.GetCount();
					
			m_pdata = new _data_type[count];
			memcpy((void*)m_pdata, (void*)other.GetData(), size);
			m_size = size;
			m_count = count;
		}
		return * this;
	}
	
	const _data_type& operator[](int nIndex) const
	{
		ASSERT((size_t )nIndex >= 0 && (size_t )nIndex < m_size);
		return m_pdata[nIndex]; 
	}
	_data_type& operator[](int nIndex)
	{
		ASSERT((size_t )nIndex >= 0 && (size_t )nIndex < m_size);
		return m_pdata[nIndex]; 
	}

private:
	_data_type* m_pdata;
	size_t m_count;
	size_t m_size;
};

