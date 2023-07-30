#include <QDBusConnection>
#include <QDBusAbstractAdaptor>
#include <QDBusInterface>
#include <QDBusVariant>

#define SERVICE_NAME "org.example.RichText"

class DBusPublisher : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)
public:
	DBusPublisher(QObject* parent);

signals:
	void event(QDBusVariant const& event);
};

class DBusSubscriber : public QDBusAbstractInterface
{
	Q_OBJECT

public:
	DBusSubscriber(QString const& service, QString const& path, QDBusConnection const& connection, QObject* parent = nullptr);

signals:
	void event(QDBusVariant const& event);
};

struct DBusSession : public QObject
{
	void sendEvent(QString const& str);

	static void createInstance(QString const& session);
	static DBusSession* instance();

signals:
	void eventReceived(QString const& str);
	void event(QDBusVariant const& event);

private:
	Q_OBJECT

	DBusSession();
	void initInstance(QString const& session);

	DBusPublisher* m_server;
	DBusSubscriber* m_client;
	QDBusConnection m_connection;

	static DBusSession* _instance;
};