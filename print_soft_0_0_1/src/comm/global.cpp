#include "global.h"



//
//HttpCommBody::HttpCommBody()
//{
//	Clear();
//}
//
//void HttpCommBody::Clear()
//{
//	strUrl.clear();
//	bAppendix = false;
//	bSerialBody = false;
//	urlData.clear();
//	heartInfo.clear();
//	bodyInfo.clear();
//	strBodyInfo.clear();
//}
//
//SerConnParam::SerConnParam()
//{
//	Clear();
//}
//
//bool SerConnParam::IsEmpty()
//{
//	if (ip.isEmpty() || port.isEmpty() || userName.isEmpty() || pwd.isEmpty())
//	{
//		return true;
//	}
//	return false;
//}
//
//void SerConnParam::Clear()
//{
//	ip.clear();
//	port.clear();
//	userName.clear();
//	pwd.clear();
//}
//
//OSSTokenParam::OSSTokenParam()
//{
//	Clear();
//}
//
//bool OSSTokenParam::IsEmpty()
//{
//	if (ossId.empty() || ossSerct.empty() || ossToken.empty() ||
//		ossBucket.empty() || ossEndPoint.empty() || ossRegin.empty() || ossExpireTime.empty())
//	{
//		return true;
//	}
//	return false;
//}
//
//void OSSTokenParam::Clear()
//{
//	ossId.clear();
//	ossSerct.clear();
//	ossToken.clear();
//	ossBucket.clear();
//	ossEndPoint.clear();
//	ossRegin.clear();
//	ossExpireTime.clear();
//}
//
//void OSSTokenParam::FromJson(const QJsonObject& jsonObj)
//{
//	ossId = jsonObj["access_key_id"].toString().toStdString();
//	ossSerct = jsonObj["access_key_secret"].toString().toStdString();
//	ossToken = jsonObj["security_token"].toString().toStdString();
//	ossBucket = jsonObj["bucket"].toString().toStdString();
//	ossEndPoint = jsonObj["endpoint"].toString().toStdString();
//	ossRegin = jsonObj["region"].toString().toStdString();
//	ossExpireTime = jsonObj["expire_time"].toVariant().toLongLong();
//}
//
//UserInfoParam::UserInfoParam()
//{
//	Clear();
//}
//
//bool UserInfoParam::IsEmpty()
//{
//	if (userId == 0 || userStaffName.empty() || userPhoneNum == 0 || usereEmail.empty() ||
//		userDepartment == 0 || userCity == 0 || userStatus == 0)
//	{
//		return true;
//	}
//	return false;
//}
//
//void UserInfoParam::Clear()
//{
//	userId = 0;
//	userStaffName.clear();
//	userPhoneNum = 0;
//	usereEmail.clear();
//	userDepartment = 0;
//	userCity = 0;
//	userStatus = 0;
//}
//
//void UserInfoParam::FromJson(const QJsonObject& jsonObj)
//{
//	userId = jsonObj["id"].toVariant().toLongLong();
//	userStaffName = jsonObj["staff_name"].toString().toStdString();
//	userPhoneNum = jsonObj["phone_number"].toVariant().toLongLong();
//	usereEmail = jsonObj["email"].toString().toStdString();
//	userDepartment = jsonObj["department"].toInt();
//	userCity = jsonObj["city"].toInt();
//	userStatus = jsonObj["status"].toInt();
//}
//
//
//BaseSerRes::BaseSerRes(int code /*= -1*/, const std::string& msg/*=""*/)
//	: status_code(code)
//	, status_msg(msg)
//{
//	err.ParseFormatMsg(msg);
//}
//
//
//void BaseSerRes::FromJosn(const QJsonObject& json)
//{
//	status_code = json["status_code"].toInt();
//	status_msg = json["status_msg"].toString().toStdString();
//	err.ParseFormatMsg(status_msg);
//}
//
//bool BaseSerRes::IsSuccess() const
//{
//	return status_code == 0;
//}
//
//std::string BaseSerRes::GetMsg() const
//{
//	return status_msg;
//}
//
//ErrInfo BaseSerRes::GetErr() const
//{
//	return err;
//}
//
//std::unique_ptr<BaseSerRes> BaseSerRes::CreateResponse(const QByteArray& jsonData)
//{
//	QJsonDocument doc = QJsonDocument::fromJson(jsonData);
//
//	if (doc.isNull() || !doc.isObject()) 
//	{
//		return std::make_unique<BaseSerRes>(-1, "Invalid JSON format");
//	}
//
//	QJsonObject json = doc.object();
//
//	// 根据JSON内容判断创建哪种类型的响应对象
//	if (json.contains("user_info") && json["user_info"].isObject()) 
//	{
//		auto response = std::make_unique<UserRes>();
//		response->FromJosn(json);
//		return response;
//	}
//	else if (json.contains("token") && json["token"].isObject()) 
//	{
//		auto response = std::make_unique<OssTokenRes>();
//		response->FromJosn(json);
//		return response;
//	}
//	else if (json.contains("service_info") && json["service_info"].isObject())
//	{
//		auto response = std::make_unique<SerOrderRes>();
//		response->FromJosn(json);
//		return response;
//	}
//	else if (json.contains("plan_list") && json["plan_list"].isArray()) 
//	{
//		auto response = std::make_unique<AlgDisposeRes>();
//		response->FromJosn(json);
//		return response;
//	}
//	else
//	{
//		auto response = std::make_unique<ErrRes>();
//		response->FromJosn(json);
//		return response;
//	}
//	// 默认返回基类
//	return std::make_unique<BaseSerRes>();
//}
//
//
//void OssTokenRes::FromJosn(const QJsonObject& json)
//{
//	BaseSerRes::FromJosn(json);  
//	if (json.contains("token") && json["token"].isObject()) 
//	{
//		ossParam.FromJson(json["token"].toObject());
//	}
//}
//
//void UserRes::FromJosn(const QJsonObject& json)
//{
//	BaseSerRes::FromJosn(json);
//	token = json["token"].toString().toStdString();
//	if (json.contains("user_info") && json["user_info"].isObject())
//	{
//		userParam.FromJson(json["user_info"].toObject());
//	}
//}
//
//
//SerProcessParam::SerProcessParam()
//{
//	sppSerId = "";
//	sppProgressNum = 0;
//	sppFileList.clear();
//	sppType = 0;
//}
//
//void SerProcessParam::FromJson(const QJsonArray& json)
//{
//	if (sppProgressNum = 2)
//	{
//		for (const auto& item : json) 
//		{
//			if (item.isObject()) 
//			{
//				QJsonObject obj = item.toObject();
//				int finger_id = obj["finger_id"].toInt();
//				std::string file_path = obj["cnc_file_path"].toString().toStdString();
//				sppFileList.emplace_back(finger_id, file_path);
//			}
//		}
//	}
//}
//
//
//std::string SerProcessParam::CreateJsonReq()
//{
//	// 上传文件
//	QJsonObject json;
//	json["service_id"] = QString::fromStdString(sppSerId);
//	if (sppType == 1)
//	{
//		QJsonArray modelArray;
//		for (auto&[fingerId, modelPath] : sppFileList) 
//		{
//			QJsonObject item;
//			item["finger_id"] = fingerId;
//			item["model_path"] = QString::fromStdString(modelPath);
//			modelArray.append(item);
//		}
//		json["model_list"] = modelArray;
//	}
//	// 状态改变
//	else if(sppType == 2)
//	{
//		json["progress"] = sppProgressNum;
//	}
//	return QJsonDocument(json).toJson(QJsonDocument::Indented).toStdString();
//}
//
//ErrInfo::ErrInfo()
//	: err_code(-1)
//	, err_msg("")
//	, bValid(false)
//{
//	
//}
//
//
//void ErrInfo::ParseFormatMsg(const std::string& msg)
//{
//	//"err_code=数字, err_msg=文本" 格式
//	std::regex pattern(R"(err_code=(\d+),\s*err_msg=([^,]+))");
//	std::smatch match;
//
//	if (std::regex_search(msg, match, pattern) && match.size() >= 3)
//	{
//		err_code = std::stoi(match[1]);
//		err_msg = match[2];
//		bValid = true;
//	}
//	else 
//	{
//		// 未匹配到特定格式，使用原始消息
//		err_msg = msg;
//		bValid = false;
//	}
//}
//
//ErrRes::ErrRes(int code, const std::string& msg)
//	: SBaseSerRes(code, msg)
//{
//
//}
//
//void ErrRes::FromJosn(const QJsonObject& json)
//{
//	SBaseSerRes::FromJosn(json);
//	err.err_code = status_code;
//	err.err_msg = status_msg;
//	err.bValid = true;
//}
//
//SerOrderInfoParam::SerOrderInfoParam()
//{
//	Clear();
//}
//
//bool SerOrderInfoParam::IsEmpty()
//{
//	//todo
//	return false;
//}
//
//void SerOrderInfoParam::Clear()
//{
//	sopId.clear();
//	sopType = 0;
//	sopProId = 0;
//	sopSerId.clear();
//	sopStoreId = 0;
//	sopSkuId = 0;
//	sopSerialNum.clear();
//	sopName.clear();
//	sopCostTtime = 0;
//	sopRemarks.clear();
//	sopSerStep = 0;
//}
//
//void SerOrderInfoParam::FromJson(const QJsonObject& json)
//{
//	sopId = json["order_id"].toString().toStdString();
//	sopType = json["order_type"].toInt();
//	sopProId = json["product_id"].toInt();
//	sopSerId = json["service_id"].toString().toStdString();
//	sopStoreId = json["store_id"].toVariant().toLongLong();
//	sopSkuId = json["sku_id"].toVariant().toLongLong();
//	sopSerialNum = json["serial_number"].toString().toStdString();
//	sopName = json["name"].toString().toLocal8Bit().toStdString();
//	sopStatus = json["status"].toInt();
//	sopCreateTime = static_cast<time_t>(json["create_time"].toVariant().toLongLong());
//	sopOrderTime = static_cast<time_t>(json["order_time"].toVariant().toLongLong());
//	sopCostTtime = json["cost_time"].toVariant().toLongLong();
//	sopRemarks = json["remarks"].toString().toStdString();
//	sopSerStep = json["step"].toInt();
//
//}
//
//void SerOrderRes::FromJosn(const QJsonObject& json)
//{
//	BaseSerRes::FromJosn(json);
//	if (json.contains("service_info") && json["service_info"].isObject())
//	{
//		serOrderParam.FromJson(json["service_info"].toObject());
//	}
//}
//
//
//
//void AlgDisposeRes::FromJosn(const QJsonObject& json)
//{
//	SBaseSerRes::FromJosn(json);
//	if (json.contains("plan_list") && json["plan_list"].isArray())
//	{
//		serData.FromJson(json["plan_list"].toArray());
//
//	}
//}
//
//std::map<EURLType, std::string> g_eURL2Str =
//{
//	{EUrl_Login, "/machine/user/login/" },{EUrl_LoginStatus, "/machine/user/" },
//	{EUrl_GetOssToken, "/machine/auth/oss_token/" },{EUrl_UploadNailData, "/machine/order/upload_nail_model/" },
//	{EUrl_GetAlgNailData, "/machine/order/get_service_plan/" },
//	{EUrl_UpdateSerStep, "/machine/order/set_service_progress/" },{EUrl_GetSerOrderDetail, "/machine/order/service_info/" }
//};

SerialParam::SerialParam()
{

}
