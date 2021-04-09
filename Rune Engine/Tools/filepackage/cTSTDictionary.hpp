#ifndef H_TST_DICTIONARY
#define H_TST_DICTIONARY

/********************************************************************
    created:    2010/11/10
    filename:   cTSTDictionary.hpp
    file path:  \Libraries\Rune Engine\Tools\filepackage
    author:     Slime Meteor
	
    purpose:    Fast Insert, Fast Find.
                String Key -> Data Pointer Mapping

    version:    0.0.1.0
*********************************************************************/

#define TSTD_DEBUG_PRINT_NEW_NODE 0
#define TSTD_DEBUG_PRINT_INSERT_TRACE 0
#define TSTD_DEBUG_PRINT_FIND_TRACE 0

#include <boost/pool/object_pool.hpp> // for object_pool
#include <vector> // for vector

// Ternary Search Tree Node
template <class T1>
struct TSTNode
{
    T1*             pstData;    // point to Data Struct in MemoryPool
    char            nodeChar;   // Node Char
    TSTNode*        pLeft;      // point to Left Node (lesser)
    TSTNode*        pMid;       // point to Mid Node (match)
    TSTNode*        pRight;     // point to Right Node (bigger)

    TSTNode()
    : pstData(NULL),
      nodeChar(0x7F),
      pLeft(NULL),
      pMid(NULL),
      pRight(NULL)
    {
    };
};

// Ternary Search Tree Dictionary
template <class T1>
class TSTDictionary
{
public:
    TSTDictionary();
    virtual ~TSTDictionary();

    void            Insert(const char* cstr, T1& data);
    T1*             Find(const char* cstr);

    unsigned int    DataSize();
    unsigned int    NodeSize();

    bool            IsEmpty();

    std::vector<T1*>&   GetTable();

protected:

private:
    TSTNode<T1>*    m_EmptyNode;
    char            m_EmptyChar;

    TSTNode<T1>*    m_RootNode;
    char            m_RootChar;
    
    unsigned int    m_TotalDataCount;
    unsigned int    m_TotalNodeCount;

    boost::object_pool< T1 >            m_dataPool;
    boost::object_pool< TSTNode<T1> >   m_nodePool;

    std::vector<T1*>                    m_vecDataList;
};

//////////////////////////////////////////////////////////////////////////

template <class T1>
TSTDictionary<T1>::TSTDictionary()
: m_EmptyChar(0x00),
  m_RootChar(0x5F),
  m_TotalDataCount(0),
  m_TotalNodeCount(0)
{
    m_EmptyNode = m_nodePool.construct();
    m_EmptyNode->nodeChar = m_EmptyChar;

    m_RootNode = m_nodePool.construct();
    m_RootNode->nodeChar = m_RootChar;
}

template <class T1>
TSTDictionary<T1>::~TSTDictionary()
{
}

template <class T1>
void TSTDictionary<T1>::Insert(const char* cstr, T1& data)
{
    TSTNode<T1>* pNode = m_RootNode;
    char* pChar = const_cast<char*>(cstr);
#if TSTD_DEBUG_PRINT_INSERT_TRACE
    OutputDebugStringA("_");
#endif
    for (;;)
    {
        if (pNode->nodeChar == '\0')
        {
            pNode->pstData = m_dataPool.construct();
            *(pNode->pstData) = data;
            m_vecDataList.push_back(pNode->pstData);
            ++m_TotalDataCount;
#if TSTD_DEBUG_PRINT_INSERT_TRACE
            OutputDebugStringA(" D\n");
#endif
            break;
        }

        if      (pNode->nodeChar < (*pChar))
        {
            if (pNode->pRight == NULL)
            {
                pNode->pRight = m_nodePool.construct();
                pNode->pRight->nodeChar = (*pChar);
                ++m_TotalNodeCount;
#if TSTD_DEBUG_PRINT_NEW_NODE
                OutputDebugStringA(" N");
#endif
            }
            pNode = pNode->pRight;
#if TSTD_DEBUG_PRINT_INSERT_TRACE
            OutputDebugStringA(" R");
#endif
            continue;
        }
        else if (pNode->nodeChar > (*pChar))
        {

            if (pNode->pLeft == NULL)
            {
                pNode->pLeft = m_nodePool.construct();
                pNode->pLeft->nodeChar = (*pChar);
                ++m_TotalNodeCount;
#if TSTD_DEBUG_PRINT_NEW_NODE
                OutputDebugStringA(" N");
#endif
            }
            pNode = pNode->pLeft;
#if TSTD_DEBUG_PRINT_INSERT_TRACE
            OutputDebugStringA(" L");
#endif
            continue;
        }
        else
        {
            ++pChar;
            if (pNode->pMid == NULL)
            {
                pNode->pMid = m_nodePool.construct();
                pNode->pMid->nodeChar = (*pChar);
                ++m_TotalNodeCount;
#if TSTD_DEBUG_PRINT_NEW_NODE
                OutputDebugStringA(" N");
#endif
            }
            pNode = pNode->pMid;
#if TSTD_DEBUG_PRINT_INSERT_TRACE
            OutputDebugStringA(" M");
#endif
            continue;
        }
    }// end for
}

template <class T1>
T1* TSTDictionary<T1>::Find(const char* cstr)
{
    TSTNode<T1>* pNode = m_RootNode;
    char* pChar = const_cast<char*>(cstr);
#if TSTD_DEBUG_PRINT_FIND_TRACE
    OutputDebugStringA("_");
#endif
    for (;;)
    {
        if (pNode == NULL)
        {
#if TSTD_DEBUG_PRINT_FIND_TRACE
            OutputDebugStringA(" X\n");
#endif
            break;
        }

        if (pNode->nodeChar == '\0')
        {
#if TSTD_DEBUG_PRINT_FIND_TRACE
            OutputDebugStringA(" D\n");
#endif
            return pNode->pstData;
        }

        if      (pNode->nodeChar < (*pChar))
        {
            pNode = pNode->pRight;
#if TSTD_DEBUG_PRINT_FIND_TRACE
            OutputDebugStringA(" R");
#endif
            continue;
        }
        else if (pNode->nodeChar > (*pChar))
        {
            pNode = pNode->pLeft;
#if TSTD_DEBUG_PRINT_FIND_TRACE
            OutputDebugStringA(" L");
#endif
            continue;
        }
        else
        {
            ++pChar;
            pNode = pNode->pMid;
#if TSTD_DEBUG_PRINT_FIND_TRACE
            OutputDebugStringA(" M");
#endif
            continue;
        }
    }// end for

    return NULL; // cannot find
}

template <class T1>
unsigned int TSTDictionary<T1>::DataSize()
{
    return m_TotalDataCount;
}

template <class T1>
unsigned int TSTDictionary<T1>::NodeSize()
{
    return m_TotalNodeCount;
}

template <class T1>
bool TSTDictionary<T1>::IsEmpty()
{
    return (m_TotalDataCount == 0) ? true : false;
}

template <class T1>
std::vector<T1*>& TSTDictionary<T1>::GetTable()
{
    return m_vecDataList;
}

#endif // H_TST_DICTIONARY
