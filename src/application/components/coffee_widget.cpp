#include "coffee_widget.h"

CoffeWidget::CoffeWidget(QWidget *parent, mqtt_client *client, QString name, QString topic) : QWidget(parent) {
    setupUi(this);
    this->name = name;
    this->topic = topic;

    this->name_text->setText(name);
    this->topic_text->setText(topic);

    connect(client, SIGNAL(getMessage(QByteArray, QString)), this, SLOT(addMessage(QByteArray, QString)));
    connect(this, SIGNAL(sendMessage(QByteArray, QString, int)), parent, SLOT(addMessage(QByteArray, QString, int)));
}

void CoffeWidget::addMessage(QByteArray msg, QString topicName) {
    QDateTime dateTime = dateTime.currentDateTime();
    QString currentTime = dateTime.toString("[HH:mm:ss] ");
    if (topicName == this->topic) {
        this->status_text->setText(currentTime + msg);
    }
}

void CoffeWidget::on_set_btn_clicked() {
    emit this->sendMessage("make", this->topic, 1);
}
void CoffeWidget::on_delete_btn_clicked() {
    delete this;
}