



//----------------------------------------------
// Loading object from GCS bare metal using HTTP (but do not seem to get all data)
//  let HTTPS_URI = "https://storage.cloud.google.com/ql-magnushyttsten.appspot.com/backend/fundinfo-db-master.bin?alt=media"
//  getGCSUsingHTTP() {
//    guard let url = URL(string: HTTPS_URI) else {
//      let s = "DBU, error at URL"
//      print(s)
//      return
//    }
//
//    URLSession.shared.dataTask(with: url) {
//      (data, response, error) in
//      //      do {
//      print("Response: \(response)")
//
//      if error != nil {
//        let s = "DBU, got error: \(error!)"
//        print(s)
//        self.message = s
//        return
//      }
//
//      guard let data=data else {
//        let s = "DBU, error at data"
//        print(s)
//        self.message = s
//        return
//      }
//
//      let s = "DBU, data success: \(data.count)"
////      self.message = s
//      print("Here is the data: \(data)")
//      print(s)
//
//      //        DispatchQueue.main.async {
//      //          self.todos = todos
//      //        }
//      //      } catch {
//      //        let s = "Failed To decode: \(error)"
//      //        messageUpdate = s
//      //        print(s)
//      //      }
//    }
//    .resume()
