#include "applications.hpp"

using namespace VPZ::APKStudio::Helpers;
using namespace VPZ::APKStudio::Resources;

namespace VPZ {
namespace APKStudio {
namespace Components {

Applications::Applications(const QString &device, QWidget *parent) :
    QTreeWidget(parent), device(device)
{
    QStringList labels;
    labels << translate("header_file");
    labels << translate("header_package");
    labels << translate("header_system");
    labels << translate("header_status");
    setHeaderLabels(labels);
    setColumnWidth(0, 160);
    setColumnWidth(1, 160);
    setColumnWidth(2, 64);
    setColumnWidth(3, 64);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setRootIsDecorated(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::MultiSelection);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
}

void Applications::onAction(QAction *action)
{
    switch (action->data().toInt())
    {
    case ACTION_DETAILS:
        onDetails();
        break;
    case ACTION_DISABLE:
        onDisable();
        break;
    case ACTION_ENABLE:
        onEnable();
        break;
    case ACTION_INSTALL:
        onInstall();
        break;
    case ACTION_PULL:
        onPull();
        break;
    case ACTION_UNINSTALL:
        onUninstall();
        break;
    }
}

void Applications::onDetails()
{
}

void Applications::onDisable()
{
    QVector<Application> applications = selected();
    if (applications.isEmpty())
        return;
    int failed = 0;
    int successful = 0;
    foreach (const Application &application, applications) {
        if (ADB::instance()->enable(device, application.package, false)) {
            successful++;
            QList<QTreeWidgetItem *> rows = findItems(application.package, Qt::MatchExactly, 1);
            if (rows.count() != 1)
                continue;
            rows.first()->setIcon(0, ::icon("cross"));
        } else
            failed++;
    }
    if (failed >= 1)
        QMessageBox::critical(this, translate("title_failure"), translate("message_disable_failed").arg(successful, failed), QMessageBox::Close);
}

void Applications::onEnable()
{
    QVector<Application> applications = selected();
    if (applications.isEmpty())
        return;
    int failed = 0;
    int successful = 0;
    foreach (const Application &application, applications) {
        if (ADB::instance()->enable(device, application.package, true)) {
            successful++;
            QList<QTreeWidgetItem *> rows = findItems(application.package, Qt::MatchExactly, 1);
            if (rows.count() != 1)
                continue;
            rows.first()->setIcon(0, ::icon("tick"));
        } else
            failed++;
    }
    if (failed >= 1)
        QMessageBox::critical(this, translate("title_failure"), translate("message_enable_failed").arg(successful, failed), QMessageBox::Close);
}

void Applications::onInitComplete()
{
    onRefresh();
}

void Applications::onInstall()
{
    QFileDialog dialog(this, translate("title_select"), Helpers::Settings::previousDirectory(), "Android APK (*.apk)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if (dialog.exec() != QFileDialog::Accepted)
        return;
    Helpers::Settings::previousDirectory(dialog.directory().absolutePath());
    QStringList files = dialog.selectedFiles();
    if (files.isEmpty())
        return;
    int failed = 0;
    int successful = 0;
    foreach (const QString &apk, files) {
        if (ADB::instance()->install(device, apk))
            successful++;
        else
            failed++;
    }
    if (failed >= 1)
        QMessageBox::critical(this, translate("title_failure"), translate("message_install_failed").arg(successful, failed), QMessageBox::Close);
    if (successful >= 1)
        onRefresh();
}

void Applications::onPull()
{
    QVector<Application> applications = selected();
    if (applications.isEmpty())
        return;
    QFileDialog dialog(this, translate("title_browse"), Helpers::Settings::previousDirectory());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec() != QFileDialog::Accepted)
        return;
    QStringList folders = dialog.selectedFiles();
    if (folders.count() != 1)
        return;
    QDir directory(folders.first());
    int failed = 0;
    int successful = 0;
    foreach (const Application &application, applications) {
        if (ADB::instance()->pull(device, application.path, directory.absolutePath())) {
            if (QFile::exists(directory.absoluteFilePath(application.name)))
                successful++;
        } else
            failed++;
    }
    if (failed >= 1)
        QMessageBox::critical(this, translate("title_failure"), translate("message_pull_failed").arg(successful, failed), QMessageBox::Close);
}

void Applications::onUninstall()
{
    QVector<Application> applications = selected();
    if (applications.isEmpty())
        return;
    int failed = 0;
    int successful = 0;
    foreach (const Application &application, applications) {
        if (ADB::instance()->uninstall(device, application.package)) {
            successful++;
            QList<QTreeWidgetItem *> rows = findItems(application.package, Qt::MatchExactly, 1);
            if (rows.count() != 1)
                continue;
            delete rows.first();
        } else
            failed++;
    }
    if (failed >= 1)
        QMessageBox::critical(this, translate("title_failure"), translate("message_uninstall_failed").arg(successful, failed), QMessageBox::Close);
}

void Applications::onRefresh()
{
    if (model()->hasChildren())
        model()->removeRows(0, model()->rowCount());
    QVector<Application> applications = ADB::instance()->applications(device);
    foreach (const Application &application, applications) {
        QTreeWidgetItem *row = new QTreeWidgetItem();
        for (int i = 0; i < 4; ++i)
            row->setData(i, ROLE_STRUCT, QVariant::fromValue(application));
        row->setIcon(0, ::icon(application.enabled ? "tick" : "cross"));
        row->setText(0, application.name);
        row->setText(1, application.package);
        row->setText(2, application.system ? "Yes" : "No");
        row->setText(3, application.enabled ? "Enabled" : "Disabled");
        row->setToolTip(0, application.path);
        addTopLevelItem(row);
    }
    scrollToTop();
}

QVector<Application> Applications::selected()
{
    QVector<Application> applications;
    foreach (QTreeWidgetItem *item, selectedItems())
        applications.append(item->data(0, ROLE_STRUCT).value<Application>());
    return applications;
}

Applications::~Applications()
{
    foreach (QMetaObject::Connection connection, connections)
        disconnect(connection);
}

} // namespace Components
} // namespace APKStudio
} // namespace VPZ