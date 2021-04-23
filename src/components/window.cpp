#include "window.h"

window::window(QWidget *parent) : QWidget(parent) {
    setupUi(this);

    ServerDialog dialog;
    auto dialog_ret = dialog.exec();
    if (dialog_ret == QDialog::Accepted) {
        client.connect(dialog.getServerAdress().toStdString(), dialog.getClientId().toStdString());
    }
    if (dialog_ret == QDialog::Rejected) {
        exit(0);
    }

    connect(&client, SIGNAL(getMessage(QByteArray)), this, SLOT(addMessage(QByteArray)));
}

// SLOTS

void window::on_send_btn_clicked() {
    auto message_data = textEdit->toPlainText().toStdString();

    if (message_data.size() == 0)
        return;

    QByteArray msg = QByteArray::fromStdString(message_data);
    msg.append(STRING_MSG);

    client.sendMessage(msg);
    this->addMessage(msg);

    textEdit->setText("");
}

void window::on_attachFile_btn_clicked() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Attach Image"), "./", tr("Images (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));

    if (fileNames.size() == 0)
        return;

    QByteArray msg;
    // open file
    std::ifstream myfile(fileNames[0].toStdString());

    // write image data to message
    char byte = 0;
    while (myfile.get(byte)) {
        msg.append(byte);
    }
    // append byte to recognize type of message - image
    msg.append(IMAGE_MSG);

    client.sendMessage(msg);
    this->addMessage(msg);
}

void window::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {

    if (item->data(Qt::UserRole).isNull())
        return;

    // load image from item data
    QPixmap img;
    img.loadFromData(item->data(Qt::UserRole).value<QByteArray>());
    // show image
    ImageForm *image = new ImageForm();
    image->SetImage(&img);
    image->setMaximumWidth(img.width());
    image->setMaximumHeight(img.height());

    image->show();
}

void window::on_subscribe_btn_clicked() {
    SubscribeDialog dialog;
    if (dialog.exec() == QDialog::Rejected)
        return;

    addNewTopic(dialog.getTopicName());
    client.subscribe(dialog.getTopicName().toStdString());
}

// CLASS FUNCTIONS

void window::addMessage(QByteArray msg) {
    QListWidgetItem *item = new QListWidgetItem();
    QIcon icon = QIcon("./img/person.ico");
    item->setIcon(icon);
    item->setFlags(Qt::ItemIsEnabled);

    // if (myMessage) {
    //     item->setBackground(Qt::gray);
    // }

    int msg_type = msg.back();
    msg.chop(1);

    switch (msg_type) {
    case IMAGE_MSG:
        item->setData(Qt::DisplayRole, "[image file]");
        item->setData(Qt::UserRole, msg);
        break;
    case STRING_MSG:
        item->setData(Qt::DisplayRole, msg);
        break;
    default:
        break;
        return;
    }

    // Add item and scroll down
    listWidget->addItem(item);
    listWidget->scrollToBottom();
}

QString getFullTopicName(QStringList list, int end) {
    QString res = "";
    for (int i = 0; i <= end; i++)
        res += (res=="" ? "" : "/") + list[i];
    return res;
}

QString getFullTopicName(QStringList list) {
    return getFullTopicName(list, list.count() - 1);
}

QTreeWidgetItem *window::findTopic(QString topicName) {
    QString server = QString::fromStdString(client.client->get_server_uri());

    QStringList topicList = topicName.split('/');

    auto leaf = topicList.last();

    QList<QTreeWidgetItem *> find_res = treeWidget->findItems(leaf, Qt::MatchExactly | Qt::MatchRecursive);
    for (auto child : find_res) {
        QString child_path = child->data(0, Qt::UserRole).value<QString>();
        std::cout << child_path.toStdString() << ":" << topicName.toStdString() << std::endl;
        if (topicName == child_path)
            return child;
    }

    return NULL;
}

QTreeWidgetItem *window::findTopicRecursive(QString topicName, int *i) {
    QStringList topicList = topicName.split('/');
    for (*i = topicList.count() - 1; *i >= 0; --*i) {
        auto res = findTopic(getFullTopicName(topicList, *i));
        if (res)
            return res;
    }
    return NULL;
}

void window::addNewTopic(QString topicName) {
    QStringList topicList = topicName.split('/');
    topicName = getFullTopicName(topicList);
    // home/kuchyna
    int i;
    QTreeWidgetItem *last = findTopicRecursive(topicName, &i);
    i++;
    for (; i < topicList.count(); i++) {
        auto new_topic = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(topicList[i]));
        new_topic->setData(0, Qt::UserRole, getFullTopicName(topicList, i));

        if (last != NULL)
            last->addChild(new_topic);
        else
            treeWidget->addTopLevelItem(new_topic);
        last = new_topic;
    }

    //    treeWidget->findItems() takto najdeme item a ptm vieme appednut child tomu itemu
    //    tu su flagy na matchovanie stringov aby sme našli spravneho potmoka podla topicu ktorý chceme pridať a prípadne aby sa spravila nova hierarchia
}

void window::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column) {
    textEdit->setEnabled(1);
    send_btn->setEnabled(1);
    attachFile_btn->setEnabled(1);
}
