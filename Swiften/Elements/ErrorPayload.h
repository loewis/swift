#pragma once

#include "Swiften/Elements/Payload.h"
#include "Swiften/Base/String.h"

namespace Swift {
	class ErrorPayload : public Payload {
		public:
			enum Type { Cancel, Continue, Modify, Auth, Wait };

			enum Condition {
				BadRequest,
				Conflict,
				FeatureNotImplemented,
				Forbidden,
				Gone,
				InternalServerError,
				ItemNotFound,
				JIDMalformed,
				NotAcceptable,
				NotAllowed,
				NotAuthorized,
				PaymentRequired,
				RecipientUnavailable,
				Redirect,
				RegistrationRequired,
				RemoteServerNotFound,
				RemoteServerTimeout,
				ResourceConstraint,
				ServiceUnavailable,
				SubscriptionRequired,
				UndefinedCondition,
				UnexpectedRequest
			};

			ErrorPayload(Condition condition = UndefinedCondition, Type type = Cancel, const String& text = String()) : type_(type), condition_(condition), text_(text) { }

			Type getType() const {
				return type_; 
			}

			void setType(Type type) {
				type_ = type;
			}

			Condition getCondition() const { 
				return condition_; 
			}

			void setCondition(Condition condition) { 
				condition_ = condition;
			}

			void setText(const String& text) {
				text_ = text;
			}

			const String& getText() const { 
				return text_; 
			}

		private:
			Type type_;
			Condition condition_;
			String text_;
	};
}