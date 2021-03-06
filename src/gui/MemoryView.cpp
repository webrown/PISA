#include "MemoryView.h"
#include "MainWindow.h"

MemoryView::MemoryView(QWidget* widget) : QTableWidget(widget){
    //Do nothing
}
MemoryView::~MemoryView(){
    //Do nothing
}
void MemoryView::init(MainWindow* main, QTableWidget* memoryTable, HexSpinBox* spinBox, QPushButton* searchButton, QLineEdit* lineEdit){
    this->main = main;
    this->memoryTable = memoryTable;
    this->spinBox = spinBox;
    this->searchButton =searchButton;
    this->lineEdit = lineEdit;

    memoryTable->setRowCount(256);
    for(int row = 0; row < 256; row++){
        memoryTable->setItem(row, 0, new QTableWidgetItem());
        memoryTable->setItem(row, 1, new QTableWidgetItem());
        memoryTable->setItem(row, 2, new QTableWidgetItem());
        memoryTable->setItem(row, 3, new QTableWidgetItem());
    }
    connect(spinBox, SIGNAL (valueChanged(int)), this, SLOT(updateWithSpinBox()));
    connect(searchButton, SIGNAL (clicked()), this, SLOT(updateWithSearch()));
    connect(memoryTable->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(handleHeader(int)));

    update();

    memoryTable->horizontalHeaderItem(2)->setText("Decimal");


}
void MemoryView::handleHeader(int x){
    if(x == 2){
        mode = static_cast<Mode>((mode +1) %3);
        update();
    }
    switch(mode){
        case DEC:
        memoryTable->horizontalHeaderItem(2)->setText("Decimal");
        break;
        case FLOAT:
        memoryTable->horizontalHeaderItem(2)->setText("Float");
        break;
        case INT:
        memoryTable->horizontalHeaderItem(2)->setText("Integer");
        break;
    };
}


void MemoryView::updateWithSpinBox(){
    qDebug() << "GUI: Update with spin box";
    uint address = spinBox->hexValue();
    main->sendMessage(ThreadMessage(ThreadMessage::R_VIEW_MEMORY, address));
}
void MemoryView::updateWithSearch(){
    qDebug() << "GUI: Update with search";
    bool okay;
    uint searchAddress = lineEdit->text().toUInt(&okay, 16);
    if(okay == false){
        return;
    }
    uint address = searchAddress >> 8;
    main->sendMessage(ThreadMessage(ThreadMessage::R_VIEW_MEMORY, address));
   
}
void MemoryView::display(QList<QVariant> list){
    uint address = list[0].toUInt();
    uint mask = (MainMemory::MEMORY_CHUNKS-1) << 8;
    uint chunkBit = ((mask & address) >> 8) << 16;
    uint x = (((1<<8)-1) & address) << 8;
    //check empty
    for(uint i = 0; i < 256; i++){
        uint content = list[i+1].toUInt();
        memoryTable->item(i, 0)->setText("0X" + QString::number(chunkBit + x + i ,16).rightJustified(8,'0'));
        memoryTable->item(i, 1)->setText("0X" + QString::number(content, 16).rightJustified(8,'0'));
        switch(mode){
            case FLOAT:
                {
                    // qDebug() << list[row+1];
                    int v = list[i+1].toUInt();
                    float * vv= (float *) & v;
                    memoryTable->item(i, 2)->setText(QString::number(*vv));
                    break;
                }
            case INT:
                {
                    int v = list[i+1].toUInt();
                    memoryTable->item(i, 2)->setText(QString::number(v));
                    break;
                }
            case DEC:
                memoryTable->item(i, 2)->setText(QString::number(list[i+1].toUInt()));
                break;
        };

        memoryTable->item(i, 3)->setText("0B" + QString::number(content, 2).rightJustified(32,'0'));
    }

    /* uint specific = searchAddress & ((1<<8)-1);
     * memoryTable->selectRow(specific); */
 
}
void MemoryView::update(){
    updateWithSpinBox();
}
