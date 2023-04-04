#include "JsonReferenceResolver.h"

#include "widgets/jsonKeys.h"

#include <QJsonObject>
#include <QUrl>

namespace
{

	//--------------------------------------------------------------------------
	QString resolve_reference_token(QString ref_token)
	{
		// Replace json pointer escape sequence (see RFC-6901 (JSON Pointer)):
		// begin with ~1 so ~01 is convert correctly to ~1
		ref_token.replace("~1", "/");
		ref_token.replace("~0", "~");

		// decode %-encoded character sequences defined in RFC-3986 (URI).
		return QUrl::fromPercentEncoding(ref_token.toUtf8());
	}

	//--------------------------------------------------------------------------
	QJsonObject merge_but_ref(QJsonObject outter, QJsonObject inner)
	{
		auto res = outter;

		for (auto it = inner.constBegin(); it != inner.constEnd(); ++it)
		{
			if (it.key() != json_keys::key_ref)
			{
				res.insert(it.key(), it.value());
			}
		}
		return res;
	}

} // namespace
//------------------------------------------------------------------------------
JsonReferenceResolver::JsonReferenceResolver()
{}

//------------------------------------------------------------------------------
void JsonReferenceResolver::setSchemaRoot(QJsonValue json)
{
	root = json;
}

//------------------------------------------------------------------------------
QJsonValue JsonReferenceResolver::resolveReference(QString path) const
{
	const auto json_reference_uri = path.section("#", 0, 0);
	if (!json_reference_uri.isEmpty()) { throw "Unsupported external uri"; }

	const auto json_reference_pointer = path.section("#", 1);

	auto res = root;
	for (auto part : json_reference_pointer.split('/', QString::SkipEmptyParts))
	{
		const QString ref_token = resolve_reference_token(part);
		bool ok = false;
		const int index = ref_token.toInt(&ok);
		if (ok) { res = res[index]; }
		else { res = res[ref_token]; }
	}
	return res;
}

//------------------------------------------------------------------------------
QJsonValue JsonReferenceResolver::resolveReference(QString path,
                                                   QJsonValue ref) const
{
	do
	{
		ref = merge_but_ref(resolveReference(path).toObject(), ref.toObject());
		path = ref[json_keys::key_ref].toString();
	} while (!path.isEmpty());
	return ref;
}
