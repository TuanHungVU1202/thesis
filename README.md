# Thesis
Everything for Thesis included code and hardware design, topic Smarthome.

#servertest2, testStreamCam1, 2 can be found in seapared respository on my gitHub.

#deploy to gcloud
1. gcloud init (if need to init settings, choosing default project, instance)
2. gcloud auth login

//start instance

//replace <thesis> included brackets with name of instance
  
3. gcloud compute instances start <thesis> 
  
//"thesisserver-2111" replace (not replace quotation mark) by project ID

4. gcloud compute --project "thesisserver-2111" ssh --zone "asia-southeast1-a" "thesis"
5. gcloud compute instances stop <thesis>


#incase need to copy DIR to gcloud

<code>gcloud compute scp --recurse  D:\testFaceReg-2 root@thesis:/nodejsProject/finalWebServer</code>

#open port on gcloud shell

<code>gcloud compute firewall-rules create <rule-name> --allow tcp:9090 --source-tags=<list-of-your-instances-names> --source-ranges=0.0.0.0/0 --description="<your-description-here>"</code>
  
<code>gcloud compute firewall-rules create allow-https --description "https server" --allow tcp:443 --format json</code>

# USE THIS method to test on gcloud

https://stackoverflow.com/questions/40696280/unsafely-treat-insecure-origin-as-secure-flag-is-not-working-on-chrome
