#include "klib_test.h"

#include <stdio.h>
#include <stdlib.h>
#include "klib/list.h"

CPPUNIT_TEST_SUITE_REGISTRATION(klibTest);

klibTest::klibTest()
{
}

klibTest::~klibTest()
{
}
    
void klibTest::setUp()
{
}

void klibTest::tearDown()
{
}

struct STestStruct
{
    struct list_head head;
    int a;
};

void klibTest::testList()
{
    // test insert
    struct list_head lst;
    INIT_LIST_HEAD(&lst);
    for (int i = 0; i < 100; ++i)
    {
        STestStruct *s = (STestStruct *)malloc(sizeof(STestStruct));
        s->a = i;
        INIT_LIST_HEAD(&s->head);
        list_add_tail(&s->head, &lst);
    }

    // test traversal
    struct list_head *pos = NULL;   
    list_for_each(pos, &lst)
    {
        STestStruct *s = list_entry(pos, STestStruct, head);
        printf("%d ", s->a);
    }
    printf("\n");
    
    while (!list_empty(&lst))
    {
        STestStruct *front = list_last_entry(&lst, STestStruct, head);
        list_del(&front->head);
        printf("%d ", front->a);
        free(front);
    }
    CPPUNIT_ASSERT(list_empty(&lst));
    printf("\n");   
}
