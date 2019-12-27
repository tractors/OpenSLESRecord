//
// Created by 惠建华 on 2019/12/25.
//

#ifndef OPENSLESRECORD_RECORDBUFFER_H
#define OPENSLESRECORD_RECORDBUFFER_H


class RecordBuffer {
public:
    //二级指针,就是一个数组
    short **buffer;
    int index = -1;

public:
    RecordBuffer(int buffersize);
    ~RecordBuffer();
    //新的buffer
    short *getRecordBuffer();
    //录制好的buffer
    short *getNowBuffer();
};


#endif //OPENSLESRECORD_RECORDBUFFER_H
