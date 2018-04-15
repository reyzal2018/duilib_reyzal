#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API STRINGorID {
public:
    STRINGorID(LPCTSTR lpString);
    STRINGorID(unsigned int nID);

    LPCTSTR m_lpstr;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class CDuiString;
class DUILIB_API CDuiPoint : public tagPOINT {
public:
    CDuiPoint();
    CDuiPoint(const POINT& src);
    CDuiPoint(long x, long y);
    CDuiPoint(LPARAM lParam);
    CDuiPoint(LPCTSTR pstrValue);
    CDuiString ToString();
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CDuiSize : public tagSIZE {
public:
    CDuiSize();
    CDuiSize(const SIZE& src);
    CDuiSize(const RECT rc);
    CDuiSize(long cx, long cy);
    CDuiSize(LPCTSTR pstrValue);
    CDuiString ToString();
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CDuiRect : public tagRECT {
public:
    CDuiRect();
    CDuiRect(const RECT& src);
    CDuiRect(long iLeft, long iTop, long iRight, long iBottom);
    CDuiRect(LPCTSTR pstrValue);
    CDuiString ToString();

    int GetWidth() const;
    int GetHeight() const;
    void Empty();
    bool IsNull() const;
    void Join(const RECT& rc);
    void ResetOffset();
    void Normalize();
    void Offset(int cx, int cy);
    void Inflate(int cx, int cy);
    void Deflate(int cx, int cy);
    void Union(CDuiRect& rc);
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CDuiString {
public:
    enum { MAX_LOCAL_STRING_LEN = 63 };

    CDuiString();
    CDuiString(const TCHAR ch);
    CDuiString(const CDuiString& src);
    CDuiString(LPCTSTR lpsz, int nLen = -1);
    ~CDuiString();
    CDuiString ToString();

    void Empty();
    int GetLength() const;
    bool IsEmpty() const;
    TCHAR GetAt(int nIndex) const;
    void Append(LPCTSTR pstr);
    void Assign(LPCTSTR pstr, int nLength = -1);
    LPCTSTR GetData() const;

    void SetAt(int nIndex, TCHAR ch);
    operator LPCTSTR() const;

    TCHAR operator[] (int nIndex) const;
    const CDuiString& operator=(const CDuiString& src);
    const CDuiString& operator=(const TCHAR ch);
    const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
    const CDuiString& CDuiString::operator=(LPCSTR lpStr);
    const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
    const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
    const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
    CDuiString operator+(const CDuiString& src) const;
    CDuiString operator+(LPCTSTR pstr) const;
    const CDuiString& operator+=(const CDuiString& src);
    const CDuiString& operator+=(LPCTSTR pstr);
    const CDuiString& operator+=(const TCHAR ch);

    bool operator == (LPCTSTR str) const;
    bool operator != (LPCTSTR str) const;
    bool operator <= (LPCTSTR str) const;
    bool operator <  (LPCTSTR str) const;
    bool operator >= (LPCTSTR str) const;
    bool operator >  (LPCTSTR str) const;

    int Compare(LPCTSTR pstr) const;
    int CompareNoCase(LPCTSTR pstr) const;

    void MakeUpper();
    void MakeLower();

    CDuiString Left(int nLength) const;
    CDuiString Mid(int iPos, int nLength = -1) const;
    CDuiString Right(int nLength) const;

    int Find(TCHAR ch, int iPos = 0) const;
    int Find(LPCTSTR pstr, int iPos = 0) const;
    int ReverseFind(TCHAR ch) const;
    int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

    int __cdecl Format(LPCTSTR pstrFormat, ...);
    int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

protected:
    LPTSTR m_pstr;
    TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};

/////////////////////////////////////////////////////////////////////////////////////
//

template<typename Ty>
class DUILIB_API CDuiArray {
public:
    CDuiArray(int iPreallocSize = 0);
    CDuiArray(const CDuiArray& src);
    ~CDuiArray();

    void Empty();
    void Resize(int iSize);
    bool IsEmpty() const;
    int Find(Ty iIndex) const;
    bool Add(Ty pData);
    bool SetAt(int iIndex, Ty pData);
    bool InsertAt(int iIndex, Ty pData);
    bool Remove(int iIndex, int iCount = 1);
    int GetSize() const;
    Ty* GetData();
    Ty GetAt(int iIndex) const;
    Ty operator[] (int nIndex) const;

protected:
    Ty* m_ppVoid;
    int m_nCount;
    int m_nAllocated;
};

template<typename Ty>
CDuiArray<Ty>::CDuiArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize) {
    assert(iPreallocSize >= 0);
    if (iPreallocSize > 0) m_ppVoid = static_cast<Ty*>(malloc(iPreallocSize * sizeof(Ty)));
}

template<typename Ty>
CDuiArray<Ty>::CDuiArray(const CDuiArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0) {
    for (int i = 0; i < src.GetSize(); i++)
        Add(src.GetAt(i));
}

template<typename Ty>
CDuiArray<Ty>::~CDuiArray() {
    if (m_ppVoid != NULL) free(m_ppVoid);
}

template<typename Ty>
void CDuiArray<Ty>::Empty() {
    if (m_ppVoid != NULL) free(m_ppVoid);
    m_ppVoid = NULL;
    m_nCount = m_nAllocated = 0;
}

template<typename Ty>
void CDuiArray<Ty>::Resize(int iSize) {
    Empty();
    m_ppVoid = static_cast<Ty*>(malloc(iSize * sizeof(Ty)));
    ::ZeroMemory(m_ppVoid, iSize * sizeof(Ty));
    m_nAllocated = iSize;
    m_nCount = iSize;
}

template<typename Ty>
bool CDuiArray<Ty>::IsEmpty() const {
    return m_nCount == 0;
}

template<typename Ty>
bool CDuiArray<Ty>::Add(Ty pData) {
    if (++m_nCount >= m_nAllocated) {
        int nAllocated = m_nAllocated * 2;
        if (nAllocated == 0) nAllocated = 11;
        Ty* ppVoid = static_cast<Ty*>(realloc(m_ppVoid, nAllocated * sizeof(Ty)));
        if (ppVoid != NULL) {
            m_nAllocated = nAllocated;
            m_ppVoid = ppVoid;
        } else {
            --m_nCount;
            return false;
        }
    }
    m_ppVoid[m_nCount - 1] = pData;
    return true;
}

template<typename Ty>
bool CDuiArray<Ty>::InsertAt(int iIndex, Ty pData) {
    if (iIndex == m_nCount) return Add(pData);
    if (iIndex < 0 || iIndex > m_nCount) return false;
    if (++m_nCount >= m_nAllocated) {
        int nAllocated = m_nAllocated * 2;
        if (nAllocated == 0) nAllocated = 11;
        Ty* ppVoid = static_cast<Ty*>(realloc(m_ppVoid, nAllocated * sizeof(Ty)));
        if (ppVoid != NULL) {
            m_nAllocated = nAllocated;
            m_ppVoid = ppVoid;
        } else {
            --m_nCount;
            return false;
        }
    }
    memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(Ty));
    m_ppVoid[iIndex] = pData;
    return true;
}

template<typename Ty>
bool CDuiArray<Ty>::SetAt(int iIndex, Ty pData) {
    if (iIndex < 0 || iIndex >= m_nCount) return false;
    m_ppVoid[iIndex] = pData;
    return true;
}

template<typename Ty>
bool CDuiArray<Ty>::Remove(int iIndex, int iCount) {
    if (iIndex < 0 || iCount <= 0 || iIndex + iCount > m_nCount) return false;
    if (iIndex + iCount < m_nCount)
        ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + iCount, (m_nCount - iIndex - iCount) * sizeof(Ty));
    m_nCount -= iCount;
    return true;
}

template<typename Ty>
int CDuiArray<Ty>::Find(Ty pData) const {
    for (int i = 0; i < m_nCount; i++) if (m_ppVoid[i] == pData) return i;
    return -1;
}

template<typename Ty>
int CDuiArray<Ty>::GetSize() const {
    return m_nCount;
}

template<typename Ty>
Ty* CDuiArray<Ty>::GetData() {
    return m_ppVoid;
}

template<typename Ty>
Ty CDuiArray<Ty>::GetAt(int iIndex) const {
    if (iIndex < 0 || iIndex >= m_nCount) return NULL;
    return m_ppVoid[iIndex];
}

template<typename Ty>
Ty CDuiArray<Ty>::operator[] (int iIndex) const {
    assert(iIndex >= 0 && iIndex < m_nCount);
    return m_ppVoid[iIndex];
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

using CDuiPtrArray = CDuiArray<LPVOID>;

//#define CDuiPtrArray CDuiArray<LPVOID>
/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CDuiValArray {
public:
    CDuiValArray(int iElementSize, int iPreallocSize = 0);
    ~CDuiValArray();

    void Empty();
    bool IsEmpty() const;
    bool Add(LPCVOID pData);
    bool Remove(int iIndex,  int iCount = 1);
    int GetSize() const;
    LPVOID GetData();

    LPVOID GetAt(int iIndex) const;
    LPVOID operator[] (int nIndex) const;

protected:
    LPBYTE m_pVoid;
    int m_iElementSize;
    int m_nCount;
    int m_nAllocated;
};

/////////////////////////////////////////////////////////////////////////////////////
//

struct TITEM;
class DUILIB_API CDuiStringPtrMap {
public:
    CDuiStringPtrMap(int nSize = 83);
    ~CDuiStringPtrMap();

    void Resize(int nSize = 83);
    LPVOID Find(LPCTSTR key, bool optimize = true) const;
    bool Insert(LPCTSTR key, LPVOID pData);
    LPVOID Set(LPCTSTR key, LPVOID pData);
    bool Remove(LPCTSTR key);
    void RemoveAll();
    int GetSize() const;
    LPCTSTR GetAt(int iIndex) const;
    LPCTSTR operator[] (int nIndex) const;

protected:
    TITEM** m_aT;
    int m_nBuckets;
    int m_nCount;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CWaitCursor {
public:
    CWaitCursor();
    ~CWaitCursor();

protected:
    HCURSOR m_hOrigCursor;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CVariant : public VARIANT {
public:
    CVariant() {
        VariantInit(this);
    }
    CVariant(int i) {
        VariantInit(this);
        this->vt = VT_I4;
        this->intVal = i;
    }
    CVariant(float f) {
        VariantInit(this);
        this->vt = VT_R4;
        this->fltVal = f;
    }
    CVariant(LPOLESTR s) {
        VariantInit(this);
        this->vt = VT_BSTR;
        this->bstrVal = s;
    }
    CVariant(IDispatch *disp) {
        VariantInit(this);
        this->vt = VT_DISPATCH;
        this->pdispVal = disp;
    }

    ~CVariant() {
        VariantClear(this);
    }
};

}// namespace DuiLib

#endif // __UTILS_H__