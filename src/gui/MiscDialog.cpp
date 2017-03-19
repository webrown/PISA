#include "MiscDialog.h"
NewCacheDialog::NewCacheDialog(Cache* topCache,  QWidget *parent )
    : QDialog( parent )
{
    _ui.setupUi( this );

    connect(_ui.nextBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](int index){ update(_ui.nextBox);});
    connect(_ui.indexSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int i){ update(_ui.indexSpin);});
    connect(_ui.offsetSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int i){ update(_ui.offsetSpin);});
    connect(_ui.waySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),[=](int i){ update(_ui.waySpin);});

    _ui.nextBox->addItem("Register", QVariant(0));
    

    int cacheN = 1;
    if(topCache == NULL){
        return;
    }

    for(Cache*  curr = topCache; curr != NULL; curr = curr->nextCache){
        list.append(curr);
        QString line = "Cache %1";
        if((curr)->type == DATA){
            line += " (D)";
        }
        else if((curr)->type == INSTRUCTION){
            line += " (I)"; 
        }
        else if((curr)->type == BOTH){
            line += " (D&I)"; 
        }
        line = line.arg(cacheN++);

        _ui.nextBox->addItem(line,QVariant(cacheN) );


        //Get last cache
        Cache* last = (curr)->nextCache; 
        if(last == NULL){
            _ui.indexSpin->setMinimum( curr->indexBits); 
            _ui.offsetSpin->setMinimum(curr->logDataWordCount);
            _ui.waySpin->setMinimum(curr->logAssociativity);
            _ui.delaySpin->setMinimum(curr->delay);
        }
    }
    _ui.nextBox->setCurrentIndex(cacheN-1);
}

NewCacheDialog::~NewCacheDialog(){
    //Do nothing
}

CacheInfo NewCacheDialog::getCacheInfo(){
    qDebug() << list.size() << endl;
    CacheInfo info;

    if(_ui.nextBox->currentIndex() != 0){
        info.prev = list.at(_ui.nextBox->currentIndex()-1);
    }

    if(_ui.nextBox->currentIndex() < list.size()){
        info.next = list.at(_ui.nextBox->currentIndex());
    }
    info.indexBits = _ui.indexSpin->value();
    info.logDataWordCount = _ui.offsetSpin->value();
    info.logAssociativity = _ui.waySpin->value();
    info.delay =_ui.delaySpin->value();
    return info;
}

void NewCacheDialog::update(QSpinBox* box){
    int total =_ui.waySpin->value() + _ui.indexSpin->value() + _ui.offsetSpin->value(); 
    if(total>32){
        box->setValue(box->value()-(total-32));
    }
    
}
void NewCacheDialog::update(QComboBox* box){
    if(box->currentIndex() != 0){
    Cache* low = list.at(box->currentIndex()-1);
    _ui.indexSpin->setMinimum( low->indexBits); 
    _ui.offsetSpin->setMinimum(low->logDataWordCount);
    _ui.waySpin->setMinimum(low->logAssociativity);
    }
    else{
    _ui.indexSpin->setMinimum(0); 
    _ui.offsetSpin->setMinimum(0);
    _ui.waySpin->setMinimum(0);
    }
    
    if(box->currentIndex() < list.size()){
    Cache* high = list.at(box->currentIndex());
    _ui.indexSpin->setMaximum( high->indexBits); 
    _ui.offsetSpin->setMaximum(high->logDataWordCount);
    _ui.waySpin->setMaximum(high->logAssociativity);
    }
    else{
    _ui.indexSpin->setMaximum( 32); 
    _ui.offsetSpin->setMaximum(32);
    _ui.waySpin->setMaximum(32);
    }
}

NewFileDialog::NewFileDialog(QString dir, QWidget *parent )
    : QDialog( parent )
{
    _ui.setupUi( this );
    connect(_ui.openButton, SIGNAL(clicked()), this, SLOT(handleOpenButton()));
    connect(_ui.randomButton, SIGNAL(clicked()), this,SLOT(handleRandomButton()));
    QString docLoc =  dir;
    _ui.lineEdit_directory->insert(docLoc); 
}


void NewFileDialog::handleOpenButton()
{
    QString dirName = QFileDialog::getExistingDirectory(
            this,
            "Open Files",
            _ui.lineEdit_directory->text()
            );
    if(!dirName.isEmpty()){
        _ui.lineEdit_directory->setText(dirName);
    }
}

void NewFileDialog::handleRandomButton()
{
    //QString name = "file" + rand();
    _ui.lineEdit_fileName->setText("file"+QString::number(rand()));
}

NewFileDialog::~NewFileDialog(){
    //nothing
}

QString NewFileDialog::getFileName(){
    QString fullPath = _ui.lineEdit_directory->text() + _ui.lineEdit_fileName->text();
    return fullPath;
}

void NewFileDialog::accept(){
    if(_ui.lineEdit_fileName->text() == ""){
       //  QErrorMessage msg(this); */
        // msg.showMessage("Empty string can't be a file name");
       // msg.exec();
        return;
    }
    QFile file(getFileName());
    if(file.exists() == true){
        QMessageBox msgBox;
        msgBox.setText(getFileName() + " already exists.");
        msgBox.setInformativeText("Do you want to proceed?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes){
            QDialog::accept();
        }
        else if (ret == QMessageBox::No){
            //Nothing
        }
        else if (ret == QMessageBox::Cancel){
            QDialog::reject();
        }
            
    }
    else{
        QDialog::accept();
    }
}

