# -*- coding: utf-8 -*- 

import urllib2

def makeUrl(key):
  return 'http://10.1.72.173/nemoread/master/result/search/api.php?type=all_uri&limit=100&classify=1&key=' + key

def writeUrl(fileName,urlList):
  if len(urlList) :
    return file(fileName,'a').writelines('\n'.join(urlList) + '\n')
  return False

def readConfFile(filePath):
  lists = []
  for url in file(filePath,'r').readlines() :
    lists.append(url.strip('\r\n'))
  return lists

def doCrawlerUrl(urlList):
  for key in urlList:
    response = urllib2.urlopen(makeUrl(key))
    url_info = response.read()
    if url_info :
      content_urls    = []
      directory_urls  = []
      imageNovel_urls = []
      url_list = url_info.split('^')
      for l in url_list:
        vals = l.split(',')  #vals[0]为url，vals[1]为pg_lvl
        if len(vals) == 2 and vals[1] != '-1' :
           if vals[1] == '1' : #1:小说正文
            content_urls.append(vals[0])
           elif vals[1] == '3' : #3:小说目录页
             directory_urls.append(vals[0])
           elif vals[1] == '2' :  #2：图片小说 
             imageNovel_urls.append(vals[0])
        if len(content_urls) >= 3 and len(directory_urls) >= 1 :
          break
      if len(content_urls) > 3 : 
        content_urls = content_urls[0:3]
      if len(directory_urls) > 1 :
        directory_urls = directory_urls[0:1]
      if len(imageNovel_urls) > 1 :
        imageNovel_urls = imageNovel_urls[0:1]
      #保存结果
      writeUrl('contentUrl.txt',content_urls)
      writeUrl('directoryUrl.txt',directory_urls)
      writeUrl('imageNovelUrl.txt',imageNovel_urls)

def main():
  domains = readConfFile('/home/zouyh/svn/shenma_dev_zxx/butterfly/conf/mode/reader/dynamic_refresh/readerapp_sites.conf')
  #domains = ['hongxiu.com','69shu.com']
  doCrawlerUrl(domains)
  print 'doCrawlerUrl finished!!!!!!!!!!!!'

if __name__ == '__main__':
  main()

