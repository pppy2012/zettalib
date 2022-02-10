#include "zthread.h"
#include "stdio.h"

kunlun::ZThread::ZThread()
{
  m_threadId = 0;
  m_state = 0;
  initAttr();
}

kunlun::ZThread::~ZThread() { pthread_attr_destroy(&m_attr); }

void kunlun::ZThread::initAttr()
{
  pthread_attr_init(&m_attr);
  int retVal = pthread_attr_setscope(&m_attr, PTHREAD_SCOPE_SYSTEM);
  if (retVal != 0)
  {
    perror("kunlun zthread init pthread attr failed: ");
    pthread_attr_destroy(&m_attr);
  }
}

void *kunlun::ZThread::threadEntry(void *handler)
{
  if (handler == nullptr)
  {
    return (void *)-1;
  }
  kunlun::ZThread *ptr = static_cast<kunlun::ZThread *>(handler);
  ptr->run();
  return (void *)0;
}

int kunlun::ZThread::start()
{
  if (m_threadId > 0)
  {
    return 0;
  }
  // create thread
  int retVal = 0;
  // set current thread under working state
  m_state = 1;
  if ((retVal = pthread_create(&m_threadId, &m_attr, &threadEntry,
                               (void *)this)) != 0)
  {
    perror("kunlun zthread create failed: ");
    pthread_attr_destroy(&m_attr);
    // set exit state
    m_state = 0;
    return -1;
  }
  return 0;
}
