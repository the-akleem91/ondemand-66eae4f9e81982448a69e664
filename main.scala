Below is the Scala code to consume the provided APIs using the `Akka HTTP` library:

```scala
import akka.actor.ActorSystem
import akka.http.scaladsl.Http
import akka.http.scaladsl.model._
import akka.http.scaladsl.unmarshalling.Unmarshal
import akka.stream.ActorMaterializer
import akka.util.ByteString
import spray.json._

import scala.concurrent.{ExecutionContextExecutor, Future}
import scala.util.{Failure, Success}

// JSON Protocol for parsing responses
object JsonProtocol extends DefaultJsonProtocol {
  implicit val createSessionResponseFormat = jsonFormat1(CreateSessionResponse)
  implicit val submitQueryResponseFormat = jsonFormat1(SubmitQueryResponse)
}

// Case classes for API responses
case class CreateSessionResponse(data: SessionData)
case class SessionData(id: String)
case class SubmitQueryResponse(data: JsValue)

object ChatApiConsumer extends App {
  implicit val system: ActorSystem = ActorSystem("ChatApiConsumer")
  implicit val materializer: ActorMaterializer = ActorMaterializer()
  implicit val executionContext: ExecutionContextExecutor = system.dispatcher

  val apiKey = "<replace_api_key>"
  val externalUserId = "<replace_external_user_id>"
  val query = "Put your query here"
  val pluginIds = List(
    "plugin-1712327325", "plugin-1713962163", "plugin-1722260873",
    "plugin-1713954536", "plugin-1713958591", "plugin-1713958830",
    "plugin-1713961903", "plugin-1713967141"
  )

  // Create Chat Session
  def createChatSession(): Future[String] = {
    val url = "https://api.on-demand.io/chat/v1/sessions"
    val body = JsObject(
      "pluginIds" -> JsArray(pluginIds.map(JsString).toVector),
      "externalUserId" -> JsString(externalUserId)
    ).toString()

    val request = HttpRequest(
      method = HttpMethods.POST,
      uri = url,
      headers = List(headers.RawHeader("apikey", apiKey)),
      entity = HttpEntity(ContentTypes.`application/json`, body)
    )

    Http().singleRequest(request).flatMap { response =>
      response.status match {
        case StatusCodes.Created =>
          Unmarshal(response.entity).to[String].map { responseBody =>
            val json = responseBody.parseJson.convertTo[CreateSessionResponse](JsonProtocol.createSessionResponseFormat)
            json.data.id
          }
        case _ =>
          Future.failed(new RuntimeException(s"Failed to create chat session: ${response.status}"))
      }
    }
  }

  // Submit Query
  def submitQuery(sessionId: String): Future[Unit] = {
    val url = s"https://api.on-demand.io/chat/v1/sessions/$sessionId/query"
    val body = JsObject(
      "endpointId" -> JsString("predefined-openai-gpt4o"),
      "query" -> JsString(query),
      "pluginIds" -> JsArray(pluginIds.map(JsString).toVector),
      "responseMode" -> JsString("sync"),
      "reasoningMode" -> JsString("medium")
    ).toString()

    val request = HttpRequest(
      method = HttpMethods.POST,
      uri = url,
      headers = List(headers.RawHeader("apikey", apiKey)),
      entity = HttpEntity(ContentTypes.`application/json`, body)
    )

    Http().singleRequest(request).flatMap { response =>
      response.status match {
        case StatusCodes.OK =>
          Unmarshal(response.entity).to[String].map { responseBody =>
            println(s"Query Response: $responseBody")
          }
        case _ =>
          Future.failed(new RuntimeException(s"Failed to submit query: ${response.status}"))
      }
    }
  }

  // Main Execution
  createChatSession().onComplete {
    case Success(sessionId) =>
      println(s"Chat session created with ID: $sessionId")
      submitQuery(sessionId).onComplete {
        case Success(_) =>
          println("Query submitted successfully.")
          system.terminate()
        case Failure(exception) =>
          println(s"Failed to submit query: ${exception.getMessage}")
          system.terminate()
      }
    case Failure(exception) =>
      println(s"Failed to create chat session: ${exception.getMessage}")
      system.terminate()
  }
}
```

### Explanation:
1. **Create Chat Session**:
   - Sends a `POST` request to the `Create Chat Session` API.
   - Extracts the `sessionId` from the response.

2. **Submit Query**:
   - Sends a `POST` request to the `Submit Query` API using the `sessionId` obtained from the previous step.

3. **Akka HTTP**:
   - Used for making HTTP requests.
   - Handles JSON serialization/deserialization using `Spray JSON`.

4. **Error Handling**:
   - Includes basic error handling for HTTP status codes.

5. **Execution Flow**:
   - First, the `createChatSession` method is called.
   - Upon success, the `submitQuery` method is invoked with the `sessionId`.

Replace `<replace_api_key>` and `<replace_external_user_id>` with actual values before running the code.