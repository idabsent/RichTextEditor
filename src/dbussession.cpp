#include "dbussession.hpp"
#include "tools.hpp"

#include <QDebug>

DBusPublisher::DBusPublisher(QObject* parent)
	: QDBusAbstractAdaptor(parent)
{
	// constructor
	setAutoRelaySignals(true);
}


DBusSubscriber::DBusSubscriber(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent)
	: QDBusAbstractInterface(service, path, SERVICE_NAME, connection, parent)
{
}

DBusSession* DBusSession::_instance{nullptr};

DBusSession::DBusSession()
	: QObject{nullptr}
	, m_server{nullptr}
	, m_client{nullptr}
	, m_connection{QDBusConnection::sessionBus()}
{	}

void DBusSession::initInstance(QString const& session)
{
	if (!m_connection.isConnected())
	{
		throw std::runtime_error{"Can't connect to DBus daemon. Check daemon state"};
	}

	m_server = new DBusPublisher{ this };
	m_client = new DBusSubscriber{ {}, {} , m_connection, this};

	connect(m_client, &DBusSubscriber::event, this, [this](QDBusVariant const& msg)
		{
			auto variant = msg.variant();
			auto sMsg = QString::fromUtf8(variant.toByteArray());
			qDebug() << FUNC_SIGN << "| bytes data received:" << sMsg;
			emit eventReceived(sMsg);
		}
	);

	if (!m_connection.registerObject("/", this))
	{
		throw std::runtime_error{"Can't register object"};
	}
}

void DBusSession::createInstance(QString const& session)
{
	_instance = new DBusSession;
	_instance->initInstance(session);
}

DBusSession* DBusSession::instance()
{
	return _instance;
}

void DBusSession::sendEvent(QString const& ev)
{
	qDebug() << FUNC_SIGN << "|" << ev;

	QVariant variant{ QByteArray::fromStdString(ev.toStdString()) };

	emit event(QDBusVariant {variant});
}