#include "weather.h"
#include <string.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPMessage.h>
#include <Poco/URI.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <memory>
#include <string>

using Poco::URI;
using Poco::JSON::Object;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;

class YaForecaster : public IForecaster {
public:
    YaForecaster(const std::string& api_key, const std::string& api_endpoint)
        : weather_uri_(api_endpoint), api_key_(api_key) {
    }

    WeatherForecast ForecastWeather(std::optional<Location> where = std::nullopt) override {
        auto session = HTTPClientSession(weather_uri_.getHost(), weather_uri_.getPort());

        auto request_uri = weather_uri_;
        if (where.has_value()) {
            request_uri.addQueryParameter("lat", std::to_string(where->lat));
            request_uri.addQueryParameter("lon", std::to_string(where->lon));
        } else {
            request_uri.addQueryParameter("lat", std::to_string(kMoscowLat));
            request_uri.addQueryParameter("lon", std::to_string(kMoscowLon));
        }

        auto request = HTTPRequest(HTTPRequest::HTTP_GET, request_uri.getPathAndQuery(),
                                   Poco::Net::HTTPMessage::HTTP_1_1);
        request.add("X-Yandex-API-Key", api_key_);
        session.sendRequest(request);

        auto response = HTTPResponse();
        auto& rs = session.receiveResponse(response);
        if (const int code = response.getStatus(); code != HTTPResponse::HTTP_OK) {
            throw YandexAPIError(code, response.getReason());
        } else {
            Poco::JSON::Parser parser;
            auto result = parser.parse(rs);

            auto fact_temp = result.extract<Object::Ptr>()->get("fact");
            double temp, feels_like;
            fact_temp.extract<Object::Ptr>()->get("temp").convert(temp);
            fact_temp.extract<Object::Ptr>()->get("feels_like").convert(feels_like);
            return WeatherForecast{temp, feels_like};
        }
    }

private:
    const URI weather_uri_;
    const std::string api_key_;
    static const double constexpr kMoscowLat = 55.751625;
    static const double constexpr kMoscowLon = 37.623718;
};

std::unique_ptr<IForecaster> CreateYandexForecaster(const std::string& api_key,
                                                    const std::string& api_endpoint) {
    return std::unique_ptr<IForecaster>(new YaForecaster(api_key, api_endpoint));
}