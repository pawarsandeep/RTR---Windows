/**
 * Created by Sandeep on 25-07-2017.
 */

var app = angular.module('shmAjs', []);

app.service('liferaftJob', ['$http',function ($http) {
    var liferaftJob ={};
    liferaftJob.data = {};
    sync: liferaftJob.createJob=function (jobData) {
        return $http.post('/rest_api/liferaft-job?_format=json', jobData).
            then(function (response) {
            if(response.status==200){
                console.log(response.data);
                return{status:"success",result:"success",data:response.data};
            }
        })
    };
    return liferaftJob;
}]);

app.service('CondemnationJob', ['$http',function ($http) {
    var CondemnationJob ={};
    CondemnationJob.data = {};

    sync: CondemnationJob.createcondemnationJob=function (jobData) {
        return $http.post('/rest_api/condemnation-job?_format=json', jobData).
        then(function (response) {
            if(response.status==200){
                console.log(response.data);

                return{status:"success",result:"success",data:response.data};
            }
        })
    };

    return CondemnationJob;
}]);

app.service('autocompleteProvider', ['$http', function ($http) {
    var autocompleteProvider = {};
    sync: autocompleteProvider.getMatched = function(searchText, type){
        var data = {};
        data.searchText = searchText;
        data.type = type;
        return $http.post('/rest_api/liferaft/autocomplete', data).
        then(function(response){
            if(response.status==200){
                console.log(response.data);

                return(response.data);
            }
        })
    };
    return autocompleteProvider;
}]);

app.service('vessel', ['$http',function ($http) {
    var vessel = {};
    vessel.getDetails = function (id) {
        return $http.get('/rest_api/vessel/' + id + '?_format=json').then(function (response) {
            if (response.status == 200) {
                console.log(response.data);

                return (response.data);
            }
        })
    };
    return vessel;
}]);

app.service('liferaft', ['$http',function ($http) {
    var liferaft ={};
    liferaft.getDetails=function (id) {
        return $http.get('/rest_api/liferaft/'+ id+'?_format=json').
        then(function (response) {
            if(response.status==200){
                console.log(response.data);
                if(response.data.gas_cylinders)
                {
                    for (var i=0;i<response.data.gas_cylinders.length;i++) {
                        response.data.gas_cylinders[i].lpt = new Date(response.data.gas_cylinders[i].lpt);
                    }
                }
                if(response.data.lifed_equipments)
                {
                    for (var i=0;i<response.data.lifed_equipments.length;i++) {
                        response.data.lifed_equipments[i].field_liferaft_expiry = new Date(response.data.lifed_equipments[i].field_liferaft_expiry);
                        response.data.lifed_equipments[i].field_liferaft_replaced_expiry = new Date(response.data.lifed_equipments[i].field_liferaft_replaced_expiry);

                    }
                }
                /*-------------------*/
                response.data.field_lr_lt_latest_date = new Date(response.data.field_lr_lt_latest_date);
                response.data.field_lr_nap_latest_date	 = new Date(response.data.field_lr_nap_latest_date	);
                response.data.field_lr_git_latest_date = new Date(response.data.field_lr_git_latest_date);
                response.data.field_lr_fs_latest_date = new Date(response.data.field_lr_fs_latest_date);
                response.data.field_lr_wp_latest_date = new Date(response.data.field_lr_wp_latest_date);
                response.data.field_lr_hru_expiry = new Date(response.data.field_lr_hru_expiry);
                /*-------------------*/
                response.data.liferaft.field_liferaft_date_inspection = new Date(response.data.liferaft.field_liferaft_date_inspection);
                response.data.liferaft.field_liferaft_previous_inspec = new Date(response.data.liferaft.field_liferaft_previous_inspec);
                response.data.liferaft.field_liferaft_date_manufacturing = new Date(response.data.liferaft.field_liferaft_date_manufacturing);
                response.data.status = response.status;
                return(response.data);
            }
        })
    };

    sync: liferaft.getMatched = function(searchText,type){
        var data = {};
        data.searchText = searchText;
		 data.type = type;
        return $http.post('/rest_api/liferaft/autocomplete', data).
            then(function(response){
                if(response.status==200){
                    response.data.status = response.status;
                    console.log(response.data);
                    return(response.data);
                }
        })
    };
    return liferaft;
}]);



app.service('editCondemnation',['$http',function ($http) {
    var editCondemnation = {};

    editCondemnation.getDetails = function(entity_id)
    {

        return $http.get('/rest_api/editCondemnation/'+ entity_id +'?_format=json').
            then(function (response) {
            if(response.status==200){
                console.log(response.data);
                response.data.field_acceptance_date = new Date(response.data.field_acceptance_date);
                for (var i=0;i<response.data.gas_cylinders.length;i++) {
                    response.data.gas_cylinders[i].lpt = new Date(response.data.gas_cylinders[i].lpt);
                }

                response.data.liferaft.field_liferaft_date_inspection = new Date(response.data.liferaft.field_liferaft_date_inspection);
                response.data.liferaft.field_liferaft_date_manufacturing = new Date(response.data.liferaft.field_liferaft_date_manufacturing);
                response.data.liferaft.field_liferaft_previous_inspec =  new Date(response.data.liferaft.field_liferaft_previous_inspec);
                response.data.status = response.status;
                return(response.data);

            }
        })

    };
    sync: editCondemnation.updateCodenmnationJob = function (jobData,entity_id) {
        var data = {};
        data.jobData = jobData;
        data.entity_id = entity_id;
        return $http.patch('/rest_api/updatecondemnation_job?_format=json',data).
            then(function (response) {
                if (response.status == 200){
                    console.log(response.data);
                    return{status:"success",result:"success",data:response.data};
                }

        })
    };
    return editCondemnation;
}]);


app.service('editLiferaft',['$http',function ($http) {
    var editLiferaft = {};

    editLiferaft.getDetails = function(entity_id)
    {

        return $http.get('/rest_api/editLiferaft/'+ entity_id +'?_format=json').
        then(function (response) {
            if(response.status==200){
                console.log(response.data);
                 if(response.data.gas_cylinders) {
                     for (var i = 0; i < response.data.gas_cylinders.length; i++) {
                         response.data.gas_cylinders[i].lpt = new Date(response.data.gas_cylinders[i].lpt);
                     }
                 }
                if(response.data.lifed_equipments)
                {
                    for (var i=0;i<response.data.lifed_equipments.length;i++) {
                        response.data.lifed_equipments[i].field_liferaft_expiry = new Date(response.data.lifed_equipments[i].field_liferaft_expiry);
                        response.data.lifed_equipments[i].field_liferaft_replaced_expiry = new Date(response.data.lifed_equipments[i].field_liferaft_replaced_expiry);

                    }
                }
                /*-------------------*/
                response.data.field_lr_lt_latest_date = new Date(response.data.field_lr_lt_latest_date);
                response.data.field_lr_nap_latest_date	 = new Date(response.data.field_lr_nap_latest_date	);
                response.data.field_lr_git_latest_date = new Date(response.data.field_lr_git_latest_date);
                response.data.field_lr_fs_latest_date = new Date(response.data.field_lr_fs_latest_date);
                response.data.field_lr_wp_latest_date = new Date(response.data.field_lr_wp_latest_date);
                response.data.field_lr_hru_expiry = new Date(response.data.field_lr_hru_expiry);
                /*-------------------*/
                if(response.data.liferaft.field_liferaft_date_inspection){
                response.data.liferaft.field_liferaft_date_inspection = new Date(response.data.liferaft.field_liferaft_date_inspection);
                }
                if(response.data.liferaft.field_liferaft_previous_inspec) {
                    response.data.liferaft.field_liferaft_previous_inspec = new Date(response.data.liferaft.field_liferaft_previous_inspec);
                }
                if(response.data.liferaft.field_liferaft_date_manufacturing) {
                    response.data.liferaft.field_liferaft_date_manufacturing = new Date(response.data.liferaft.field_liferaft_date_manufacturing);
                }
               if(response.data.invoice) {
                    response.data.invoice.field_invoice_date = new Date(response.data.invoice.field_invoice_date);
                }
                     response.data.status = response.status;
                return(response.data);

            }
        })

    };
    sync: editLiferaft.updateLiferaftJob = function (jobData,entity_id) {
        var data = {};
        data.jobData = jobData;
        data.entity_id = entity_id;
        return $http.patch('/rest_api/updateliferaft_job?_format=json',data).
        then(function (response) {
            if (response.status == 200){
                console.log(response.data);
                return{status:"success",result:"success",data:response.data};
            }

        })
    };
    return editLiferaft;
}]);

app.service('serviceengineer', ['$http',function ($http) {
    var serviceengineer = {};
    serviceengineer.getDetails = function (id) {
        return $http.get('/rest_api/serviceengineer/' + id + '?_format=json').then(function (response) {
            if (response.status == 200) {
                console.log(response.data);

                return (response.data);
            }
        })
    };
    return serviceengineer;
}]);

app.run(function ($http,$window){
    $http.defaults.headers.common['X-CSRF-Token'] = $window.drupalSettings.shm_ajs.csrf_token;
    //$http.defaults.withCredentials = false;
    console.log("shmAjs Started.");
});