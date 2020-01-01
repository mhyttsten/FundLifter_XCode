//
//  AppDelegate.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/7/19.
//  Copyright © 2019 Magnus Hyttsten. All rights reserved.
//

import Firebase
import UIKit
import BackgroundTasks

@UIApplicationMain
class AppDelegate: UIResponder, UIApplicationDelegate {

  let appRefreshTask = "com.pf.fundlifter.appRefreshTask"  // max 30s    BG,FB,idle,BG,FG,idle,...,charging
  let backgroundTask = "com.pf.fundlifter.backgroundTask"  // minutes ok
  func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
    FirebaseApp.configure()
    Analytics.setAnalyticsCollectionEnabled(false)
    print("FirebaseApp.configure has been called")
    
    logFileAppend(s: "Ent application(dFWLO)")
    print("Logfile path: \(logFilePath)")
    print("Content of file is now")
    print(logFileRead())

    BGTaskScheduler.shared.register(forTaskWithIdentifier: appRefreshTask, using: nil) { task in
      // Downcast the parameter to an app refresh task as this identifier is used for a refresh request.
      self.executeAppRefreshTask(task: task as! BGAppRefreshTask)
    }
    
    BGTaskScheduler.shared.register(forTaskWithIdentifier: backgroundTask, using: nil) { task in
      // Downcast the parameter to a processing task as this identifier is used for a processing request.
      self.executeBackgroundTask(task: task as! BGProcessingTask)
    }
    
    scheduleAppRefreshTask()
    scheduleBackgroundTask()
    logFileAppend(s: "Exit app(dFWLO), success")
    return true
  }

  func applicationDidEnterBackground(_ application: UIApplication) {
      logFileAppend(s: "Ent app(dFWLO)")
      scheduleAppRefreshTask()
      scheduleBackgroundTask()
  }

  func scheduleAppRefreshTask() {
    logFileAppend(s: "Ent sART")
    let request = BGAppRefreshTaskRequest(identifier: appRefreshTask)
    request.earliestBeginDate = Date(timeIntervalSinceNow: 15 * 60)  // At least 15 minutes from now
    
    do {
      try BGTaskScheduler.shared.submit(request)
    } catch let error {
      logFileAppend(s: "*** ERR ART.submit: \(error)")
    }
    logFileAppend(s: "Exit sART success")
  }
  
  func scheduleBackgroundTask() {
    logFileAppend(s: "Ent sBT")
    let request = BGProcessingTaskRequest(identifier: backgroundTask)
    request.earliestBeginDate = Date(timeIntervalSinceNow: 60 * 60)  // At least 15 minutes from now
    request.requiresNetworkConnectivity = false
    request.requiresExternalPower = false
    
    do {
      try BGTaskScheduler.shared.submit(request)
    } catch let error {
      logFileAppend(s: "*** ERR BT.submit: \(error)")
    }
    logFileAppend(s: "Exit sBT success")
  }
  
  func executeAppRefreshTask(task: BGAppRefreshTask) {
    logFileAppend(s: "Ent eART")
    scheduleAppRefreshTask()
    
    let queue = OperationQueue()
    queue.maxConcurrentOperationCount = 1
        
    task.expirationHandler = {
      // After all operations are cancelled, the completion block below is called to set the task to complete.
      logFileAppend(s: "*** eART, timer expired")
      queue.cancelAllOperations()
    }
    
    let operations = [AppRefreshTaskOperation()]
    operations.last!.completionBlock = {
      logFileAppend(s: "*** eART completed, s: \(operations.last!.isCancelled)")
      task.setTaskCompleted(success: operations.last!.isCancelled)
      logFileAppend(s: "...done task.setTaskCompleted")
    }
    
    logFileAppend(s: "...doing queue.addOperation")
    queue.addOperations(operations, waitUntilFinished: false)
    logFileAppend(s: "Exit eART success")
  }
  
  func executeBackgroundTask(task: BGProcessingTask) {
    logFileAppend(s: "Ent eBT")
    // Should we not do scheduleBackgroundTask() here as we do in executeAppRefreshTask?

    let queue = OperationQueue()
    queue.maxConcurrentOperationCount = 1
    
    task.expirationHandler = {
      // After all operations are cancelled, the completion block below is called to set the task to complete.
      logFileAppend(s: "*** eBT, expiration")
      queue.cancelAllOperations()
    }
    
    let operations = [BackgroundTaskOperation()]
    operations.last!.completionBlock = {
      logFileAppend(s: "eBT completed, s: \(operations.last!.isCancelled)")
      task.setTaskCompleted(success: operations.last!.isCancelled)
    }
    
    queue.addOperations(operations, waitUntilFinished: false)
    logFileAppend(s: "Exit eBT success")
  }

  // To force a start of the background task
  // 1. Have phone connected to XCode
  // 2. Run your app, then pause it, then run in the debugger the following:
  //    e -l objc — (void)[[BGTaskScheduler sharedScheduler] _simulateLaunchForTaskWithIdentifier:@”com.pf.fundlifter.dbrefresh”]
  // 3. To force early termination
  //    e -l objc — (void)[[BGTaskScheduler sharedScheduler] _simulateExpirationForTaskWithIdentifier:@”com.pf.fundlifter.dbrefresh”]
  
  //---------------------------------------------------------------------------
  // MARK: UISceneSession Lifecycle

  func application(_ application: UIApplication, configurationForConnecting connectingSceneSession: UISceneSession, options: UIScene.ConnectionOptions) -> UISceneConfiguration {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return UISceneConfiguration(name: "Default Configuration", sessionRole: connectingSceneSession.role)
  }

  func application(_ application: UIApplication, didDiscardSceneSessions sceneSessions: Set<UISceneSession>) {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
  }

}

class AppRefreshTaskOperation: Operation {
  override func main() {
    logFileAppend(s: "AddRefreshTaskOperation CALLED")
  }
}

class BackgroundTaskOperation: Operation {
  override func main() {
    logFileAppend(s: "BackgroundTaskOperation CALLED")
  }
}


