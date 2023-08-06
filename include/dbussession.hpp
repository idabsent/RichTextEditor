#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusVariant>

#include "actions.hpp"

#define SERVICE_NAME "org.example.RichText"
#define INTERFACE_NAME "org.example.RichText.events"

class DBusPublisher : public QDBusAbstractAdaptor
{
	Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME)
public:
	DBusPublisher(QObject* parent);

signals:
	void action(QDBusVariant const& action);
};

class DBusSubscriber : public QDBusAbstractInterface
{
	Q_OBJECT

public:
	DBusSubscriber(QString const& service, QString const& path, QDBusConnection const& connection, QObject* parent = nullptr);

signals:
	void action(QDBusVariant const& action);
};

struct DBusSession : public QObject
{
	void sendAction(ActionUP action);
	void sendString(QString const& str);

	static void createDetached();
	static void createDisabled();
	static void createCommon();
	static void createSession(QString const& session);
	static DBusSession* instance();

signals:
	void actionReceived(ActionType action, QByteArray const& raw);

signals: //Only for internal use
	void action(QDBusVariant const& action);

private slots:
	void parseMessage(QDBusVariant const& msg);

private:
	Q_OBJECT

    DBusSession();
	void initInstance(QString const& session);

	QDBusVariant packPackage(QByteArray const& data, int appId, int actionType);

	DBusPublisher* m_server;
	DBusSubscriber* m_client;
	QDBusConnection m_connection;

	int m_appId;

	static DBusSession* _instance;
};
