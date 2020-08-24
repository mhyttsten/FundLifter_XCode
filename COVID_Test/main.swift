//
//  main.swift
//  FundLifter
//
//  Created by Magnus Hyttsten on 7/13/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

import Foundation

struct CountryData {
  var countryName: String
  var continentExp: String
  var popData2019: Int
  var weekData: [Int: CountryDataWeek] = [Int: CountryDataWeek]()
  var weekDataOrderedList: [CountryDataWeek] { return weekData.sorted { $0.0 < $1.0 }.map { $0.value } }
  var dates: [String: Void] = [String: Void]()  // To validate we don't have duplicates
}

struct CountryDataWeek {
  var weekOfYear: Int
  var cases: Int = 0
  var deaths: Int = 0
//  var cumulativeCasesFor14DaysPer100k: Double = 0.0
}

var country2Data = [String: CountryData]()
let s = FileOSUtils.fileRead(fileName: "/Users/magnushyttsten/Downloads/download")  // https://data.europa.eu/euodp/en/data/dataset/covid-19-coronavirus-data
let df = txtSeparate(data:s!, separator: ",")
for (idx, r) in df.enumerated() {
  if idx == 0 {
    continue
  }
  if r.count == 0 || r.count == 1 {
    break
  }
  
  // Problematic countries (no population or name contains comma)
  if r[6].contains("Bonaire") || r[6].contains("Cases_on") {
    continue
  }

  // Avoid duplicates
  if let cd = country2Data[r[6]] {
    if let d = cd.dates[r[0]] {
      fatalError("Found duplicate date for country \(d)")
    }
    country2Data[r[6]]!.dates[r[0]] = nil
  } else {
    let cd = CountryData(countryName: r[6],
                         continentExp: r[10],
                         popData2019: Int(r[9])!)
    country2Data[r[6]] = cd
  }

  // Add new values for this week
  let d = string2Date(dateString: r[0], dateFormat: "dd/MM/yyyy")
  let woy = weekOfYear(date: d)
  if country2Data[r[6]]?.weekData[woy] == nil {
    country2Data[r[6]]?.weekData[woy] = CountryDataWeek(weekOfYear: woy)
  }
  var cdwd = country2Data[r[6]]!.weekData[woy]!
  cdwd.cases = cdwd.cases + Int(r[4])!
//  cdwd.cumulativeCasesFor14DaysPer100k = cdwd.cumulativeCasesFor14DaysPer100k + Double(r[11])!
  cdwd.deaths = cdwd.deaths + Int(r[5])!

  country2Data[r[6]]!.weekData[woy]!.cases = country2Data[r[6]]!.weekData[woy]!.cases + Int(r[4])!
  //  cdwd.cumulativeCasesFor14DaysPer100k = cdwd.cumulativeCasesFor14DaysPer100k + Double(r[11])!
  country2Data[r[6]]!.weekData[woy]!.deaths = country2Data[r[6]]!.weekData[woy]!.deaths + Int(r[5])!
}
  
// Report
let countries = ["Sweden"]
for c in countries {
  let cd = country2Data[c]!
  print("\(cd.countryName) ", terminator: "")
  print("\(cd.weekDataOrderedList.map{ "\($0.weekOfYear):\($0.cases)/\($0.deaths)" }),  ")
  
  let a = cd.weekDataOrderedList.map { ($0.weekOfYear, $0)}
}




print("Done")

